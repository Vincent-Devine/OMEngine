#include <Core/Log/Logger.hpp>

#include <chrono>
#include <print>
#include <Windows.h>

namespace OM::Core::Log
{
	const std::string_view VerbosityToString(EVerbosity verbosity)
	{
		switch (verbosity)
		{
		case EVerbosity::VerbosityDebug:	return "[DEBUG]";
		case EVerbosity::VerbosityInfo:		return "[INFO]";
		case EVerbosity::VerbosityWarning:	return "[WARNING]";
		case EVerbosity::VerbosityError:	return "[ERROR]";
		case EVerbosity::VerbosityCritical:	return "[CRITICAL]";
		default:							return "[UNKNOWN]";
		}
	}

	const std::string_view LogTagToString(ELogTag tag)
	{
		switch (tag)
		{
		case ELogTag::TagNone:		return "[NONE]";
		case ELogTag::TagCore:		return "[CORE]";
		case ELogTag::TagLog:		return "[LOG]";
		case ELogTag::TagEditor:	return "[EDITOR]";
		default:					return "[UNKNOWN]";
		}
	}

	void Logger::Destroy()
	{
		CloseLogFile();
		delete s_instance;
		s_instance = nullptr;
	}

	void Logger::OpenLogFile(const std::filesystem::path& path, const bool overwrite)
	{
		std::scoped_lock lock(_mutex);
		if (_logFile.is_open())
		{
			std::string log = "Log file: " + _nameLogFile + " is already open. Switch to this log file: " + path.filename().string();
			InternalLog(EVerbosity::VerbosityInfo, ELogTag::TagLog, __FILE__, __func__, __LINE__, log);
			_logFile.close();
		}

		std::ios::openmode openmode = std::ios::out;
		openmode |= overwrite ? std::ios::trunc : std::ios::app;
		_logFile.open(path, openmode);
		_nameLogFile = path.filename().string();
		
		std::string log = "Open log file: " + _nameLogFile;
		InternalLog(EVerbosity::VerbosityDebug, ELogTag::TagLog, __FILE__, __func__, __LINE__, log);
	}

	void Logger::CloseLogFile()
	{
		std::string log = "Close log file: " + _nameLogFile;
		InternalLog(EVerbosity::VerbosityDebug, ELogTag::TagLog, __FILE__, __func__, __LINE__, log);
		std::scoped_lock lock(_mutex);
		if (_logFile.is_open())
			_logFile.close();
	}

	void Logger::Log(const EVerbosity verbosity, const ELogTag tag, const std::string_view& file, const std::string_view& function, const int line, const std::string_view& log)
	{
		#if defined(OM_RELEASE)

		if (!(verbosity & EVerbosity::VerbosityCritical))
			return;

		#elif defined(OM_DEVELOPMENT)

		if (!(verbosity & EVerbosity::VerbosityCritical) && !(verbosity & EVerbosity::VerbosityError) && !(verbosity & EVerbosity::VerbosityWarning))
			return;

		#endif

		if (!(_verbosity & verbosity))
			return;

		std::string logInfo;
		AddDateAndTime(logInfo);
		AddThread(logInfo);
		AddFileInfo(logInfo, file, function, line);
		AddVerbosity(logInfo, verbosity);
		AddTag(logInfo, tag);

		std::scoped_lock lock(_mutex);
		PrintConsole(logInfo, log, verbosity);
		WriteFile(std::format("{} {}", logInfo, log));
	}

	void Logger::AddDateAndTime(std::string& logInfo) const
	{
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		std::chrono::zoned_time localTime{ std::chrono::current_zone(), now };

		if ((_displaySettings & EDisplaySettings::DisplayDate) && (_displaySettings & EDisplaySettings::DisplayTime))
			logInfo += std::format("[{:%Y-%m-%d %H:%M:%S}] ", localTime);
		else if (_displaySettings & EDisplaySettings::DisplayDate)
			logInfo += std::format("[{:%Y-%m-%d}] ", localTime);
		else if (_displaySettings & EDisplaySettings::DisplayTime)
			logInfo += std::format("[{:%H:%M:%S}] ", localTime);
	}

	void Logger::AddThread(std::string& logInfo) const
	{
		if (_displaySettings & EDisplaySettings::DisplayThread)
			logInfo += std::format("[Thread ID {}] ", std::this_thread::get_id());
	}

	void Logger::AddFileInfo(std::string& logInfo, const std::string_view& file, const std::string_view& function, const int line) const
	{
		if (_displaySettings & EDisplaySettings::DisplayFileInfo)
		{
			uint64 lastSlash = file.find_last_of("\\/");
			std::string_view filename = (lastSlash != std::string_view::npos) ? file.substr(lastSlash + 1) : file;
			logInfo += std::format("[{}:{}@{}] ", filename, function, line);
		}
	}

	void Logger::AddVerbosity(std::string& logInfo, const EVerbosity verbosity) const
	{
		if (_displaySettings & EDisplaySettings::DisplayVerbosity)
			logInfo += std::format("{} ", VerbosityToString(verbosity));
	}

	void Logger::AddTag(std::string& logInfo, const ELogTag tag) const
	{
		if (_displaySettings & EDisplaySettings::DisplayTag)
			logInfo += std::format("{}", LogTagToString(tag));
	}

	void Logger::PrintConsole(const std::string_view& logInfo, const std::string_view& log, const EVerbosity verbosity) const
	{
		static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		WORD color = 7;
		switch (verbosity)
		{
		case EVerbosity::VerbosityDebug:	color = 13; break;
		case EVerbosity::VerbosityInfo:		color = 11; break;
		case EVerbosity::VerbosityWarning:	color = 14; break;
		case EVerbosity::VerbosityError:	color = 12; break;
		case EVerbosity::VerbosityCritical:	color = 79; break;
		default: break;
		}

		FILE* stream = (verbosity == EVerbosity::VerbosityError || verbosity == EVerbosity::VerbosityCritical) ? stderr : stdout;

		SetConsoleTextAttribute(handle, color);
		std::print(stream, "{}", logInfo);
		SetConsoleTextAttribute(handle, 7);
		std::println(stream, " {}", log);
	}

	void Logger::WriteFile(const std::string_view& logInfoAndLog)
	{
		if (_logFile.is_open())
			_logFile << logInfoAndLog << '\n';
	}

	void Logger::InternalLog(const EVerbosity verbosity, const ELogTag tag, const std::string_view& file, const std::string_view& function, const int line, const std::string_view& log)
	{
		#if defined(OM_DEBUG)

		if (!(_verbosity & verbosity))
			return;

		std::string logInfo;
		AddDateAndTime(logInfo);
		AddThread(logInfo);
		AddFileInfo(logInfo, file, function, line);
		AddVerbosity(logInfo, verbosity);
		AddTag(logInfo, tag);

		PrintConsole(logInfo, log, verbosity);
		WriteFile(std::format("{} {}", logInfo, log));

		#endif
	}
}
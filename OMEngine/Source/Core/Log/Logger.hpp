#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <mutex>

#include <Utils/Type.hpp>

namespace OM::Core::Log
{
	enum EVerbosity : uint8
	{
		VerbosityNone		= 0,
		VerbosityDebug		= 1 << 0,
		VerbosityInfo		= 1 << 1,
		VerbosityWarning	= 1 << 2,
		VerbosityError		= 1 << 3,
		VerbosityCritical	= 1 << 4,
		VerbosityAll = VerbosityDebug | VerbosityInfo | VerbosityWarning | VerbosityError | VerbosityCritical
	};
	const std::string_view VerbosityToString(EVerbosity verbosity);

	enum ELogTag : uint8
	{
		TagNone,
		TagCore,
		TagLog,
		TagEditor,
	};
	const std::string_view LogTagToString(ELogTag tag);

	enum EDisplaySettings : uint8
	{
		DisplayNone			= 0,
		DisplayDate			= 1 << 0,
		DisplayTime			= 1 << 1,
		DisplayThread		= 1 << 2,
		DisplayFileInfo		= 1 << 3,
		DisplayVerbosity	= 1 << 4,
		DisplayTag			= 1 << 5,
		DisplayAll			= DisplayDate | DisplayTime | DisplayThread | DisplayFileInfo | DisplayVerbosity | DisplayTag,
		DisplayOMPreset		= DisplayTime | DisplayFileInfo | DisplayVerbosity | DisplayTag
	};

	// singleton design pattern
	class Logger
	{
	public:
		static Logger* GetInstance()
		{
			if (!s_instance)
				s_instance = new Logger();
			return s_instance;
		}

		void Destroy();

		void OpenLogFile(const std::filesystem::path& path, bool overwrite = true);
		void CloseLogFile();
		
		void Log(const EVerbosity verbosity, const ELogTag tag, const std::string_view& file, const std::string_view& function, const int line, const std::string_view& log);

	private:
		static inline Logger* s_instance = nullptr;

		std::mutex _mutex;
		std::ofstream _logFile;
		EVerbosity _verbosity = EVerbosity::VerbosityAll;
		EDisplaySettings _displaySettings = EDisplaySettings::DisplayOMPreset;
		std::string _nameLogFile;

		void AddDateAndTime(std::string& logInfo) const;
		void AddThread(std::string& logInfo) const;
		void AddFileInfo(std::string& logInfo, const std::string_view& file, const std::string_view& function, const int line) const;
		void AddVerbosity(std::string& logInfo, const EVerbosity verbosity) const;
		void AddTag(std::string& logInfo, const ELogTag tag) const;

		void PrintConsole(const std::string_view& logInfo, const std::string_view& log, const EVerbosity verbosity) const;
		void WriteFile(const std::string_view& logInfoAndLog);

		void InternalLog(const EVerbosity verbosity, const ELogTag tag, const std::string_view& file, const std::string_view& function, const int line, const std::string_view& log);

		void SetDisplaySettings(const EDisplaySettings displaySettings) { _displaySettings = displaySettings; }
		void SetVerbosity(const EVerbosity verbosity) { _verbosity = verbosity; }
	};
}

#if defined(OM_DEBUG)

#define OM_LOG_DEBUG(log, tag)		OM::Core::Log::Logger::GetInstance()->Log(OM::Core::Log::EVerbosity::VerbosityDebug,	tag, __FILE__, __func__, __LINE__, log);
#define OM_LOG_INFO(log, tag)		OM::Core::Log::Logger::GetInstance()->Log(OM::Core::Log::EVerbosity::VerbosityInfo,		tag, __FILE__, __func__, __LINE__, log);
#define OM_LOG_WARNING(log, tag)	OM::Core::Log::Logger::GetInstance()->Log(OM::Core::Log::EVerbosity::VerbosityWarning,	tag, __FILE__, __func__, __LINE__, log);
#define OM_LOG_ERROR(log, tag)		OM::Core::Log::Logger::GetInstance()->Log(OM::Core::Log::EVerbosity::VerbosityError,	tag, __FILE__, __func__, __LINE__, log);

#define OM_LOG_DEBUG_CUSTOM(log, tag, file, function, line)		OM::Core::Log::Logger::GetInstance()->Log(OM::Core::Log::EVerbosity::VerbosityDebug,	tag, file, function, line, log);
#define OM_LOG_INFO_CUSTOM(log, tag, file, function, line)		OM::Core::Log::Logger::GetInstance()->Log(OM::Core::Log::EVerbosity::VerbosityInfo,		tag, file, function, line, log);
#define OM_LOG_WARNING_CUSTOM(log, tag, file, function, line)	OM::Core::Log::Logger::GetInstance()->Log(OM::Core::Log::EVerbosity::VerbosityWarning,	tag, file, function, line, log);
#define OM_LOG_ERROR_CUSTOM(log, tag, file, function, line)		OM::Core::Log::Logger::GetInstance()->Log(OM::Core::Log::EVerbosity::VerbosityError,	tag, file, function, line, log);

#elif defined (OM_DEVELOPMENT)

#define OM_LOG_DEBUG(log, tag)		do {} while(0);
#define OM_LOG_INFO(log, tag)		do {} while(0);
#define OM_LOG_WARNING(log, tag)	OM::Core::Log::Logger::GetInstance()->Log(OM::Core::Log::EVerbosity::VerbosityWarning,	tag, __FILE__, __func__, __LINE__, log);
#define OM_LOG_ERROR(log, tag)		OM::Core::Log::Logger::GetInstance()->Log(OM::Core::Log::EVerbosity::VerbosityError,	tag, __FILE__, __func__, __LINE__, log);

#define OM_LOG_DEBUG_CUSTOM(log, tag, file, function, line)		do {} while(0);
#define OM_LOG_INFO_CUSTOM(log, tag, file, function, line)		do {} while(0);
#define OM_LOG_WARNING_CUSTOM(log, tag, file, function, line)	OM::Core::Log::Logger::GetInstance()->Log(OM::Core::Log::EVerbosity::VerbosityWarning,	tag, file, function, line, log);
#define OM_LOG_ERROR_CUSTOM(log, tag, file, function, line)		OM::Core::Log::Logger::GetInstance()->Log(OM::Core::Log::EVerbosity::VerbosityError,	tag, file, function, line, log);

#else

#define OM_LOG_DEBUG(log, tag)		do {} while(0);
#define OM_LOG_INFO(log, tag)		do {} while(0);
#define OM_LOG_WARNING(log, tag)	do {} while(0);
#define OM_LOG_ERROR(log, tag)		do {} while(0);

#define OM_LOG_DEBUG_CUSTOM(log, tag, file, function, line)		do {} while(0);
#define OM_LOG_INFO_CUSTOM(log, tag, file, function, line)		do {} while(0);
#define OM_LOG_WARNING_CUSTOM(log, tag, file, function, line)	do {} while(0);
#define OM_LOG_ERROR_CUSTOM(log, tag, file, function, line)		do {} while(0);

#endif

#define OM_LOG_CRITICAL(log, tag)								OM::Core::Log::Logger::GetInstance()->Log(OM::Core::Log::EVerbosity::VerbosityCritical, tag, __FILE__, __func__, __LINE__, log);
#define OM_LOG_CRITICAL_CUSTOM(log, tag, file, function, line)	OM::Core::Log::Logger::GetInstance()->Log(OM::Core::Log::EVerbosity::VerbosityCritical, tag, file, function, line, log);

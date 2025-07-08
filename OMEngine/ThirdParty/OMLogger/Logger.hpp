#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <filesystem>
#include <mutex>
#include <thread>
#include <chrono>
#include <Windows.h>

namespace OM::Logger
{
	enum LogVerbosity : uint8_t
	{
		VerbosityNone		= 0,
		VerbosityDebug		= 1 << 0,
		VerbosityInfo		= 1 << 1,
		VerbosityWarning	= 1 << 2,
		VerbosityError		= 1 << 3,
		VerbosityCritical	= 1 << 4,
		VerbosityAll = VerbosityDebug | VerbosityInfo | VerbosityWarning | VerbosityError | VerbosityCritical
	};

	inline const char* LogVerbosityToString(uint8_t verbosity)
	{
		switch (verbosity)
		{
		case LogVerbosity::VerbosityDebug:		return "[DEBUG]";
		case LogVerbosity::VerbosityInfo:		return "[INFO]";
		case LogVerbosity::VerbosityWarning:	return "[WARNING]";
		case LogVerbosity::VerbosityError:		return "[ERROR]";
		case LogVerbosity::VerbosityCritical:	return "[CRITICAL]";
		default:								return "[UNKNOWN]";
		}
	}

	enum LogDisplaySettings : uint8_t
	{
		DisplayNone			= 0,
		DisplayDate			= 1 << 0,
		DisplayThread		= 1 << 1,
		DisplayFileInfo		= 1 << 2,
		DisplayVerbosity	= 1 << 3,
		DisplayTag			= 1 << 4,
		DisplayAll = DisplayDate | DisplayThread | DisplayFileInfo | DisplayVerbosity | DisplayTag
	};

	enum LogTag
	{
		TagNone,
		TagEngine,
		TagCore,
		TagRender,
		TagInput,
		TagPhysics,
		TagAudio,
		TagAnimation,
		TagUI,
		TagAI,
		TagResource,
		TagScripting,
		TagTools,
		TagScene,
	};

	inline const char* LogTagToString(LogTag tag)
	{
		switch (tag)
		{
		case LogTag::TagNone:		return "[NONE]";
		case LogTag::TagEngine:		return "[ENGINE]";
		case LogTag::TagCore:		return "[CORE]";
		case LogTag::TagRender:		return "[RENDER]";
		case LogTag::TagInput:		return "[INPUT]";
		case LogTag::TagPhysics:	return "[PHYSICS]";
		case LogTag::TagAudio:		return "[AUDIO] ";
		case LogTag::TagAnimation:	return "[ANIMATION]";
		case LogTag::TagUI:			return "[UI]";
		case LogTag::TagAI:			return "[AI]";
		case LogTag::TagResource:	return "[RESOURCE]";
		case LogTag::TagScripting:	return "[SCRIPTING]";
		case LogTag::TagTools:		return "[TOOLS]";
		case LogTag::TagScene:		return "[SCENE]";
		default:					return "[UNKNOWN]";
		}
	}

	class Logger
	{
		// Attributes
	private:
		static inline Logger* s_instance = nullptr;

		std::mutex m_mutex;
		std::ofstream m_logFile;
		uint8_t m_verbosity = LogVerbosity::VerbosityAll;
		uint8_t m_displaySettings = LogDisplaySettings::DisplayAll;

		// Methods
	public:
		static Logger* GetInstance()
		{
			if (!s_instance)
				s_instance = new Logger();
			return s_instance;
		}

		void Destroy()
		{
			CloseLogFile();
			delete GetInstance();
			s_instance = nullptr;
		}

		void OpenLogFile(const std::filesystem::path& path)
		{
			std::scoped_lock lock(m_mutex);
			if (m_logFile.is_open())
				m_logFile.close();
			m_logFile.open(path, std::ios::out | std::ios::app);
		}

		void CloseLogFile()
		{
			if (m_logFile.is_open())
				m_logFile.close();
		}

		void Log(uint8_t verbosity, const char* file, int line, const char* function, const LogTag tag, const std::string& message)
		{
			if (!(m_verbosity & verbosity))
				return;

			std::ostringstream logInfo;

			if (m_displaySettings & LogDisplaySettings::DisplayDate)
			{
				auto now = std::chrono::system_clock::now();
				auto timeTNow = std::chrono::system_clock::to_time_t(now);
				std::tm localTime;
				localtime_s(&localTime, &timeTNow);
				logInfo << '[' << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "] ";
			}

			if(m_displaySettings & LogDisplaySettings::DisplayThread)
				logInfo << "[Thread ID " << std::this_thread::get_id() << "] ";

			if (m_displaySettings & LogDisplaySettings::DisplayFileInfo)
			{
				const char* filename = std::strrchr(file, '\\');
				filename = filename ? filename + 1 : file;
				logInfo << '[' << filename << ':' << function << '@' << line << "] ";
			}

			if(m_displaySettings & LogDisplaySettings::DisplayVerbosity)
				logInfo << LogVerbosityToString(verbosity);

			if (tag != LogTag::TagNone && m_displaySettings & LogDisplaySettings::DisplayTag)
			{
				if (m_displaySettings & LogDisplaySettings::DisplayVerbosity)
					logInfo << ' ';

				 logInfo << LogTagToString(tag);
			}

			PrintConsole(logInfo.str(), ' ' + message, verbosity);
			WriteFile(logInfo.str() + ' ' + message);
		}

		void SetDisplaySettings(uint8_t displaySettings) { m_displaySettings = displaySettings; }
		void SetVerbosity(uint8_t verbositys) { m_verbosity = verbositys; }

		void SetOMProfile()
		{
			m_displaySettings = LogDisplaySettings::DisplayDate
				| LogDisplaySettings::DisplayFileInfo
				| LogDisplaySettings::DisplayVerbosity
				| LogDisplaySettings::DisplayTag;

			m_verbosity = LogVerbosity::VerbosityAll;
		}

	private:
		void PrintConsole(const std::string& logInfo, const std::string& message, const uint8_t verbosity)
		{
			static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
			std::scoped_lock lock(m_mutex);

			WORD color = 7; // default grey
			switch (verbosity)
			{
			case LogVerbosity::VerbosityDebug:		color = 13; break;	// purple
			case LogVerbosity::VerbosityInfo:		color = 11; break;	// cyan
			case LogVerbosity::VerbosityWarning:	color = 14; break;	// yellow
			case LogVerbosity::VerbosityError:		color = 12; break;	// red
			case LogVerbosity::VerbosityCritical:	color = 79; break; 	// white on red background
			default: break;
			}

			if (verbosity == VerbosityError || verbosity == VerbosityCritical)
			{
				SetConsoleTextAttribute(handle, color);
				std::cerr << logInfo;
				SetConsoleTextAttribute(handle, 7);
				std::cerr << message << std::endl;
			}
			else
			{
				SetConsoleTextAttribute(handle, color);
				std::cout << logInfo;
				SetConsoleTextAttribute(handle, 7);
				std::cout << message << std::endl;
			}
		}

		void WriteFile(const std::string& message)
		{
			std::scoped_lock lock(m_mutex);
			if (m_logFile.is_open())
				m_logFile << message << '\n';
		}
	};
}

// Print
#define OM_LOG_DEBUG(message)	OM::Logger::Logger::GetInstance()->Log(OM::Logger::VerbosityDebug,		__FILE__, __LINE__, __func__, OM::Logger::TagNone ,message);
#define OM_LOG_INFO(message)	OM::Logger::Logger::GetInstance()->Log(OM::Logger::VerbosityInfo,		__FILE__, __LINE__, __func__, OM::Logger::TagNone ,message);
#define OM_LOG_WARNING(message)	OM::Logger::Logger::GetInstance()->Log(OM::Logger::VerbosityWarning,	__FILE__, __LINE__, __func__, OM::Logger::TagNone ,message);
#define OM_LOG_ERROR(message)	OM::Logger::Logger::GetInstance()->Log(OM::Logger::VerbosityError,		__FILE__, __LINE__, __func__, OM::Logger::TagNone ,message);
#define OM_LOG_CRITICAL(message)OM::Logger::Logger::GetInstance()->Log(OM::Logger::VerbosityCritical,	__FILE__, __LINE__, __func__, OM::Logger::TagNone ,message);

#define OM_LOG_DEBUG_TAG(message, tag)		OM::Logger::Logger::GetInstance()->Log(OM::Logger::VerbosityDebug,		__FILE__, __LINE__, __func__, tag, message);
#define OM_LOG_INFO_TAG(message, tag)		OM::Logger::Logger::GetInstance()->Log(OM::Logger::VerbosityInfo,		__FILE__, __LINE__, __func__, tag, message);
#define OM_LOG_WARNING_TAG(message, tag)	OM::Logger::Logger::GetInstance()->Log(OM::Logger::VerbosityWarning,	__FILE__, __LINE__, __func__, tag, message);
#define OM_LOG_ERROR_TAG(message, tag)		OM::Logger::Logger::GetInstance()->Log(OM::Logger::VerbosityError,		__FILE__, __LINE__, __func__, tag, message);
#define OM_LOG_CRITICAL_TAG(message, tag)	OM::Logger::Logger::GetInstance()->Log(OM::Logger::VerbosityCritical,	__FILE__, __LINE__, __func__, tag, message);

// Assertion
#ifdef _DEBUG
#define OM_ASSERTION(expression, message) if(!(expression)) { OM_LOG_CRITICAL(message); __debugbreak(); }
#else
#define OM_ASSERTION(expression, message) if(!(expression)) { OM_LOG_CRITICAL(message); abort(); }
#endif
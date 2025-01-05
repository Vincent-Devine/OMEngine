#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <iostream>
#include <Windows.h>
#include <debugapi.h>
#include <format>
#include <chrono>

namespace OM
{
    namespace Logger
    {
        enum class ELevel : unsigned int
        {
            Info,
            Debug,
            Warning,
            Error,
        };

        void DebugLog(const std::string& fileName, const unsigned int line, const std::string& functionName, const std::string& logMessage, const ELevel logLevel);
    }
}

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG_INFO(logMessage) OM::Logger::DebugLog(__FILENAME__, __LINE__, __func__, logMessage, OM::Logger::ELevel::Info);
#define LOG_DEBUG(logMessage) OM::Logger::DebugLog(__FILENAME__, __LINE__, __func__, logMessage, OM::Logger::ELevel::Debug);
#define LOG_WARNING(logMessage) OM::Logger::DebugLog(__FILENAME__, __LINE__, __func__, logMessage, OM::Logger::ELevel::Warning);
#define LOG_ERROR(logMessage) OM::Logger::DebugLog(__FILENAME__, __LINE__, __func__, logMessage, OM::Logger::ELevel::Error);

#ifdef DEBUG
#define Assertion(expression, logMessage) if(!(expression)) { LOG_ERROR(logMessage); __Debugbreak(); }
#else
#define Assertion(expression, logMessage) if(!(expression)) { LOG_ERROR(logMessage); abort(); }
#endif

namespace OM
{
    namespace Logger
    {
        static inline std::ofstream _logFile;

	    static void OpenFile(const std::filesystem::path& fileName)
	    {
		    if (_logFile && _logFile.is_open())
            {
                DebugLog(__FILENAME__, __LINE__, __func__, "Log file already open.", ELevel::Warning);
                return;
            }

            _logFile.open(fileName, std::ios::out);
            if (!_logFile)
            {
                DebugLog(__FILENAME__, __LINE__, __func__, "Fail to open log file " + fileName.string() + ".", ELevel::Warning);
                return;
            }
            else
            {
                DebugLog(__FILENAME__, __LINE__, __func__, "Open log file " + fileName.string() + ".", ELevel::Info);
            }
        }

        static void CloseFile()
        {
            _logFile.close();
            DebugLog(__FILENAME__, __LINE__, __func__, "Close log file.", ELevel::Info);
        }

        static void print(const std::string& logMessage, const ELevel& logLevel)
        {
            HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
            std::string logMessageWithLogLevel;

            switch (logLevel)
            {
            case ELevel::Info:
            {
                SetConsoleTextAttribute(handle, 11); // texte in blue
                std::cout << "[Info]   ";
                SetConsoleTextAttribute(handle, 15); // texte in white (default)
                std::cout << logMessage << std::endl;
                logMessageWithLogLevel = "[Info] " + logMessage + '\n';
                break;
            }

            case ELevel::Debug:
            {
                SetConsoleTextAttribute(handle, 05); // texte in green
                std::cout << "[Debug]  ";
                SetConsoleTextAttribute(handle, 15); // texte in white (default)
                std::cout << logMessage << std::endl;
                logMessageWithLogLevel = "[Debug] " + logMessage + '\n';
                break;
            }

            case ELevel::Warning:
                SetConsoleTextAttribute(handle, 14); // texte in yellow
                std::cout << "[Warning]";
                SetConsoleTextAttribute(handle, 15); // texte in white (default)
                std::cout << logMessage << std::endl;
                logMessageWithLogLevel = "[Warning] " + logMessage + '\n';
                break;

            case ELevel::Error:
                SetConsoleTextAttribute(handle, 207); // background in red, texte in white
                std::cout << "[Error]";
                SetConsoleTextAttribute(handle, 15); // texte in white (default)
                std::cout << "  " + logMessage << std::endl;
                logMessageWithLogLevel = "[Error] " + logMessage + '\n';
                break;

            default:
                std::cout << logMessage << std::endl;;
                logMessageWithLogLevel = logMessage + '\n';
                break;
            }

            if (_logFile.is_open())
            {
                _logFile << logMessageWithLogLevel;
            }
        }

        inline void DebugLog(const std::string& fileName, const unsigned int line, const std::string& functionName, const std::string& logMessage, const ELevel logLevel)
        {
            std::string time = std::format("{:%T}", floor<std::chrono::seconds>(std::chrono::system_clock::now()));
            std::string message = "[" + time + "]" + fileName + '(' + std::to_string(line) + "):" + functionName + ": " + logMessage;
            OutputDebugStringA(message.c_str());
            print(message, logLevel);
        }
    }
}
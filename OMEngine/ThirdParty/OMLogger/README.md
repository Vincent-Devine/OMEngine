# OMLogger

![Header-Only](https://img.shields.io/badge/header--only-blue)
![Version](https://img.shields.io/badge/version-2.1.0-blue)
![C++20](https://img.shields.io/badge/C%2B%2B-20-orange)
![Windows](https://img.shields.io/badge/platform-Windows-lightgrey)
![License: MIT](https://img.shields.io/badge/license-MIT-green)

**OMLogger** (One More Logger) is a single-header **logging library** for C++20, designed for clarity, flexibility, and ease of use.  
It supports **console and file outputs**, **verbosity filtering**, **color-coded messages**, **tag-based categorization**, and more — all with zero dependencies.

![png](./Screenshot/Exemple.png)

## Table of Contents
- [Features](#features)
- [Add to your project](#add-to-your-project)
- [Usage](#usage)
  - [Basic logging](#basic-logging)
  - [Verbosity levels](#verbosity-levels)
  - [Tags](#tags)
  - [Display Settings](#display-settings)
  - [File output](#file-output)
  - [Custom data](#custom-data)
  - [Assertion](#assertion)
- [Requirements](#requirements)
- [Credit](#credit)

## Features
- Console output with **colored logs**
- Optional **file output**
- Configurable **verbosity levels**
- **Tag system** for filtering and categorizing logs
- **Assertion** macro with custom error messages
- Lightweight and **header-only**

## Add to your project  
Add the [`OMLogger/Logger.hpp`](./OMLogger/Logger.hpp) file to your project and include it where needed.  
```cpp
#include "OMLogger/Logger.hpp"
```   
There are no external dependencies.

## Usage

### Basic logging
Use the macros to log your messages:
```cpp
int main()
{
	OM_LOG_DEBUG("your text here");
	OM_LOG_INFO("your text here");
	OM_LOG_WARNING("your text here");
	OM_LOG_ERROR("your text here");
	OM_LOG_CRITICAL("your text here");

	OM::Logger::Logger::GetInstance()->Destroy(); // Important!
}
```

### Verbosity Levels
Use the `SetVerbosity()` method to filter log messages:
```cpp
int main()
{
	OM::Logger::Logger::GetInstance()->SetVerbosity(OM::Logger::VerbosityDebug | OM::Logger::VerbosityInfo);
	OM_LOG_DEBUG("Only debug and info verbosity");		// Displayed
	OM_LOG_INFO("Only debug and info verbosity");		// Displayed 
	OM_LOG_WARNING("Only debug and info verbosity");	// Not displayed
	OM_LOG_ERROR("Only debug and info verbosity");		// Not displayed
	OM_LOG_CRITICAL("Only debug and info verbosity");	// Not displayed
}
```

### Tags
**Tags**: Engine, Core, Render, Input, Physics, Audio, Animation, UI, AI, Resource, Scripting, Tools, Scene<br> 
Use the macros to log and tag your messages:
```cpp
int main()
{
	OM_LOG_INFO_TAG("your text here", OM::Logger::TagCore);
}
```

### Display Settings
**Settings**: Date, Time, Thread, FileInfo, Verbosity, Tag <br>
Use the `SetDisplaySettings()` method to configure what log information is shown:
```cpp
int main()
{
	OM::Logger::Logger::GetInstance()->SetDisplaySettings(OM::Logger::DisplayTime);
	OM_LOG_INFO("Displays only date + message");

	OM::Logger::Logger::GetInstance()->SetDisplaySettings(OM::Logger::DisplayDate | OM::Logger::DisplayTime | OM::Logger::DisplayFileInfo);
	OM_LOG_INFO("Displays date + time + file info + message");
}
```

Result:  
![png](./Screenshot/DisplaySettings.png)

### File Output
Use the `OpenLogFile()` and `CloseLogFile()` methods to enable or disable file output:
```cpp
int main()
{
	// Beginning of the project
	OM::Logger::Logger* logger = OM::Logger::Logger::GetInstance();
	logger->OpenLogFile("log.txt");

	OM_LOG_DEBUG("Message written to console and log file");

	// End of the project
	logger->CloseLogFile();
}
```
By default, writing to the file will overwrite the existing contents of the log file. You can pass an additional parameter when calling the function to keep the existing entries instead of deleting them.
```cpp
logger->OpenLogFile("log.txt", false);
```

### Custom Data
To manually specify the file, function, and line, consider using helper functions. Make sure that the log entry corresponds to the function calling the helper, not the helper itself.
```cpp
void Helper(std::string message, const char* file, int line, const char* function)
{
	OM_LOG_INFO_CUSTOM_DATA(message, OM::Logger::TagNone, file, line, function);
}
#define CUSTOM_LOG(message) Helper(message, __FILE__, __LINE__, __func__)

int main()
{
	// file, line and func are on the main and not on the Helper() function
	CUSTOM_LOG("Custom log");
}
```

### Assertion
Use the `OM_ASSERTION` macro to assert your code:
```cpp
int main()
{
	// If _DEBUG is defined: logs a critical message + triggers a breakpoint
	// Else: logs a critical message + aborts

	OM_ASSERTION(1 + 1 == 2, "your text here"); // Passes
	OM_ASSERTION(1 + 1 == 3, "your text here"); // Fails
}
```
Assertions support **tags** `OM_ASSERTION_TAG` and **custom data** `OM_ASSERTION_CUSTOM_DATA`, allowing you to categorize failures and specify the file, line, and function manually when needed.

## Requirements
- **C++20**
- Windows OS *(for console color support via WinAPI)*

## Credit
Made by: [Vincent DEVINE](https://github.com/Vincent-Devine)
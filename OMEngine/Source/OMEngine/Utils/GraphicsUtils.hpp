#pragma once

#include <comdef.h>
#include <OMLogger/Logger.hpp>

namespace OM::Utils
{
	inline std::string GetErrorMsg(HRESULT result)
	{
		_com_error error(result);
		return std::string(error.ErrorMessage(), error.ErrorMessage() + wcslen(error.ErrorMessage()));
	}

	inline bool CheckHResult(HRESULT result, std::string message, const char* file, int line, const char* function)
	{
		if (SUCCEEDED(result))
			return true;

		OM_LOG_CRITICAL_CUSTOM_DATA(message + " - " + GetErrorMsg(result), OM::Logger::TagRender, file, line, function);
		return false;
	}
	#define CHECK_HRESULT(result, message) OM::Utils::CheckHResult(result, message, __FILE__, __LINE__, __func__)

	inline bool CheckHResultAssert(HRESULT result, std::string message, const char* file, int line, const char* function)
	{
		if (SUCCEEDED(result))
			return true;

		OM_ASSERTION_CUSTOM_DATA(false, message + " - " + GetErrorMsg(result), OM::Logger::TagRender, file, line, function);
		return false;
	}
	#define OM_ASSERTION_HRESULT(result, message) OM::Utils::CheckHResultAssert(result, message, __FILE__, __LINE__, __func__)
}
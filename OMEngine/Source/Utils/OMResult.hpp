#pragma once

#include <string>

#include <Utils/Type.hpp>
#include <Core/Log/Assertion.hpp>
#include <Core/Log/Logger.hpp>

namespace OM
{
	enum EResult : uint8
	{
		ResultSuccess,
		ResultFail
	};

	struct [[nodiscard]] OMResult
	{
		const EResult _result;
		const std::string_view _message;

		OMResult(const EResult result, const std::string_view& message)
			: _result(result)
			, _message(message)
		{
		}
	};
}

namespace OM::Utils
{
	inline bool CheckOMResult(const OMResult& result, const std::string_view& customMessage, const Core::Log::EVerbosity verbosity, const Core::Log::ELogTag tag, const std::string_view& file, const std::string_view& function, const int line)
	{
		if (result._result == EResult::ResultSuccess)
			return true;

		const std::string log = std::string(customMessage).append(" - ").append(result._message);
		OM_LOG(log, verbosity, tag, file, function, line);
		return false;
	}

	inline bool AssertionOMResult(const OMResult& result, const std::string_view& customMessage, const Core::Log::ELogTag tag, const std::string_view& file, const std::string_view& function, const int line)
	{
		if (result._result == EResult::ResultSuccess)
			return true;

		const std::string log = std::string(customMessage).append(" - ").append(result._message);
		OM_ASSERTION_CUSTOM(false, log, tag, file, function, line);
		return false;
	}
}

#define CHECK_OMRESULT(result, message, verbosity, tag) OM::Utils::CheckOMResult(result, message, verbosity, tag, __FILE__, __func__, __LINE__);
#define ASSERTION_OMRESULT(result, message, tag)		OM::Utils::AssertionOMResult(result, message, tag, __FILE__, __func__, __LINE__);
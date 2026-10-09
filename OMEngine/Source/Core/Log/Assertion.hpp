#pragma once

#include <Core/Log/Logger.hpp>

#if defined(OM_DEBUG)

#define OM_ASSERTION(expression, log, tag) if (!(expression)) { OM_LOG_CRITICAL(log, tag); __debugbreak(); }
#define OM_ASSERTION_CUSTOM(expression, log, tag, file, function, line) if (!(expression)) { OM_LOG_CRITICAL_CUSTOM(log, tag, file, function, line); __debugbreak(); }

#else

#define OM_ASSERTION(expression, log, tag) if (!(expression)) { OM_LOG_CRITICAL(log, tag); abort(); }
#define OM_ASSERTION_CUSTOM(expression, log, tag, file, function, line) if (!(expression)) { OM_LOG_CRITICAL_CUSTOM(log, tag, file, function, line); abort(); }

#endif
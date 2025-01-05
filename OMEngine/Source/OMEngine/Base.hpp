#pragma once

#ifdef OM_ENGINE_EXPORTS
#define OM_ENGINE_API __declspec(dllexport)
#else
#define OM_ENGINE_API __declspec(dllimport)
#endif

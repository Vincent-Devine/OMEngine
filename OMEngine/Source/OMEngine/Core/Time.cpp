#include "pch.h"
#include "OMEngine/Core/Time.hpp"
#include <OMLogger/Logger.hpp>

namespace OM::Core
{
	double SystemTime::s_cpuTickDelta = 0.0;

	void SystemTime::Initialisation()
	{
		LARGE_INTEGER frequency;
		OM_ASSERTION(QueryPerformanceFrequency(&frequency) == TRUE, "Unable to query performance counter frequency");
		s_cpuTickDelta = 1.0 / static_cast<double>(frequency.QuadPart);
	}

	__int64 SystemTime::GetCurrentTick()
	{
		LARGE_INTEGER currentTick;
		OM_ASSERTION(QueryPerformanceCounter(&currentTick) == TRUE, "Unable to query performance counter value");
		return static_cast<__int64>(currentTick.QuadPart);
	}

	void SystemTime::BusyLoopSleep(float sleepTime)
	{
		__int64 finalTick = static_cast<__int64>(static_cast<double>(sleepTime) / s_cpuTickDelta) + GetCurrentTick();
		while (GetCurrentTick() < finalTick);
	}
}

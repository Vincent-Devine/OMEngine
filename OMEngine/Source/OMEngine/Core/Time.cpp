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

	long long SystemTime::GetCurrentTick()
	{
		LARGE_INTEGER currentTick;
		OM_ASSERTION(QueryPerformanceCounter(&currentTick) == TRUE, "Unable to query performance counter value");
		return static_cast<long long>(currentTick.QuadPart);
	}

	void SystemTime::BusyLoopSleep(float sleepTime)
	{
		long long finalTick = static_cast<long long>(static_cast<double>(sleepTime) / s_cpuTickDelta) + GetCurrentTick();
		while (GetCurrentTick() < finalTick);
	}
}

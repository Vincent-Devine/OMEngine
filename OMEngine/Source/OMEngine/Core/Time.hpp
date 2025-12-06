#pragma once

#include "OMEngine/Base.hpp"

namespace OM::Core
{
	class OM_ENGINE_API SystemTime
	{
	public:
		static void Initialisation();
		static long long GetCurrentTick();
		static void BusyLoopSleep(float sleepTime);

		static inline double TickToSeconds(long long tickCount)
		{
			return tickCount * s_cpuTickDelta;
		}

		static inline double TickToMillisecs(long long tickCount)
		{
			return tickCount * s_cpuTickDelta * 1000.0;
		}

		static inline double TimeBetweenTicks(long long tick1, long long tick2)
		{
			return TickToSeconds(tick2 - tick1);
		}

	private:
		static double s_cpuTickDelta;
	};

	class OM_ENGINE_API CpuTimer
	{
	public:
		CpuTimer()
		{
			_startTick = 0ll;
			_elapsedTicks = 0ll;
		}

		void Start()
		{
			if (_startTick == 0ll)
				_startTick = SystemTime::GetCurrentTick();
		}

		void Stop()
		{
			if (_startTick != 0ll)
			{
				_elapsedTicks += SystemTime::GetCurrentTick() - _startTick;
				_startTick = 0ll;
			}
		}

		void Reset()
		{
			_startTick = 0ll;
			_elapsedTicks = 0ll;
		}

		double GetTime() const
		{
			return SystemTime::TickToSeconds(_elapsedTicks);
		}

	private:
		long long _startTick;
		long long _elapsedTicks;
	};
}
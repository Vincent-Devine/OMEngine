#pragma once

#include "OMEngine/Base.hpp"
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>

namespace OM::Core
{
	class OM_ENGINE_API ThreadPool
	{
	public:
		static ThreadPool* GetInstance();

		bool Initialisation(unsigned int nbThreadCreated = 5);
		void AddTask(std::function<void()> task);
		void Destroy();

	private:
		static inline ThreadPool* _instance = nullptr;

		std::vector<std::thread> _workers;
		std::queue<std::function<void()>> _tasks;
		std::mutex _mutexTasks;
		std::condition_variable _conditionVariable;
		std::atomic<bool> _stopFlag;
	};
}


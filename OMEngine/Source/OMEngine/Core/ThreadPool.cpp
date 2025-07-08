#include "pch.h"

#include "OMEngine/Core/ThreadPool.hpp"

namespace OM::Core
{
	/*
	ThreadPool* ThreadPool::GetInstance()
	{
		if (!_instance)
			_instance = new ThreadPool();
		return _instance;
	}

	bool ThreadPool::Initialisation(unsigned int nbThreadCreated)
	{
		for (unsigned int i = 0; i < nbThreadCreated; i++)
		{
			_workers.emplace_back([this] {
				while (true)
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock(_mutexTasks);
						_conditionVariable.wait(lock, [this] { return _stopFlag || !_tasks.empty(); });
						if (_stopFlag && _tasks.empty())
							return;

						task = std::move(_tasks.front());
						_tasks.pop();
					}
					task();
				}
			});
		}
		return true;
	}

	void ThreadPool::AddTask(std::function<void()> task)
	{
		std::lock_guard<std::mutex> lock(_mutexTasks);
		_tasks.emplace(task);
		_conditionVariable.notify_one();
	}

	void ThreadPool::Destroy()
	{
		std::lock_guard<std::mutex> lock(_mutexTasks);
		_stopFlag = true;

		_conditionVariable.notify_all();
		for (std::thread& worker : _workers)
		{
			if (worker.joinable())
				worker.join();
		}

		_workers.clear();
		delete _instance;
	}
	*/
}

#include "pch.h"
#include "OMEngine/Application.hpp"
#include <OMLogger/Logger.hpp>
#include "OMEngine/Core/ThreadPool.hpp"

namespace OM
{
	bool Application::Initialisation()
	{
		Logger::OpenFile("OMEngine.log");

		// if (!Core::ThreadPool::GetInstance()->Initialisation(2))
		// 	return false;

		LOG_INFO("Initialisation complete.");
		return true;
	}

	void Application::Update()
	{
	}

	void Application::Render() const
	{
	}

	void Application::Destroy()
	{
		// Core::ThreadPool::GetInstance()->Destroy();
		LOG_INFO("Destroy complete.");
		Logger::CloseFile();
	}

	const bool Application::WindowShouldClose() const
	{
		return true;
	}
}
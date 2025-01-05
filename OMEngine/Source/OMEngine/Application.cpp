#include "pch.h"
#include "OMEngine/Application.hpp"
#include <OMLogger/Logger.hpp>

namespace OM
{
	bool Application::Initialisation()
	{
		Logger::OpenFile("OMEngine.log");

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
		LOG_INFO("Destroy complete.");
		Logger::CloseFile();
	}

	const bool Application::WindowShouldClose() const
	{
		return true;
	}
}
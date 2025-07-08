#include "pch.h"
#include "OMEngine/Application.hpp"
#include <OMLogger/Logger.hpp>
#include "OMEngine/Core/ThreadPool.hpp"

namespace OM
{
	bool Application::Initialisation(HINSTANCE hInstance, int nCmdShow)
	{
		OM::Logger::Logger::GetInstance()->OpenLogFile("OMEngine.log");
		OM::Logger::Logger::GetInstance()->SetDisplaySettings(OM::Logger::DisplayFileInfo | OM::Logger::DisplayVerbosity | OM::Logger::DisplayTag);

		_window = Wrapper::Window::GetInstance();
		if (!_window->Initialisation(hInstance, nCmdShow))
			return false;

		OM_LOG_INFO_TAG("Initialisation complete.", OM::Logger::TagCore);
		return true;
	}

	void Application::Update()
	{
		_window->Update();
	}

	void Application::Render() const
	{
		_window->Render();
	}

	void Application::Destroy()
	{
		_window->Destroy();
		OM_LOG_INFO("Destroy complete.");
		OM::Logger::Logger::GetInstance()->CloseLogFile();
	}

	const bool Application::WindowShouldClose() const
	{
		return _window->GetWindowShouldClose();
	}
}
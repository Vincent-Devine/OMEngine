#include "pch.h"
#include "OMEngine/Application.hpp"
#include <OMLogger/Logger.hpp>
#include "OMEngine/Core/ThreadPool.hpp"

namespace OM
{
	bool Application::Initialisation(HINSTANCE hInstance)
	{
		Logger::OpenFile("OMEngine.log");

		// if (!Core::ThreadPool::GetInstance()->Initialisation(2))
		// 	return false;

		_window = Wrapper::Window::GetInstance();
		if (!_window->Initialisation(hInstance))
			return false;

		LOG_INFO("Initialisation complete.");
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
		// Core::ThreadPool::GetInstance()->Destroy();
		LOG_INFO("Destroy complete.");
		Logger::CloseFile();
	}

	const bool Application::WindowShouldClose() const
	{
		return _window->GetWindowShouldClose();
	}
}
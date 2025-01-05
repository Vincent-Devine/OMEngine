#include "pch.h"
#include "OMEngine/Application.hpp"
#include <iostream>

namespace OM
{
	bool Application::Initialisation()
	{
		std::cout << "Initialisation" << std::endl;
		return true;
	}

	void Application::Update()
	{
		std::cout << "Update" << std::endl;
	}

	void Application::Render() const
	{
		std::cout << "Render" << std::endl;
	}

	void Application::Destroy()
	{
		std::cout << "Destroy" << std::endl;
	}

	const bool Application::WindowShouldClose() const
	{
		return true;
	}
}
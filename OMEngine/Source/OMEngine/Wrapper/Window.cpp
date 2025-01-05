#include "pch.h"

#include "OMEngine/Wrapper/Window.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <OMLogger/Logger.hpp>
#include "OMEngine/Utils.hpp"

namespace OM::Wrapper
{
	Window* Window::GetInstance()
	{
		if (!_instance)
			_instance = new Window();
		return _instance;
	}

	bool Window::Initialisation()
	{
		glfwSetErrorCallback(DisplayError);

		if (glfwInit() == GLFW_FALSE)
		{
			LOG_ERROR("GLFW couldn't initalited.");
			return false;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLFW_MAJOR_VERSION);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLFW_MINOR_VERSION);

		_window = glfwCreateWindow(Utils::SCREEN_WIDTH, Utils::SCREEN_HEIGHT, Utils::GAME_NAME, NULL, NULL);

		if (!_window)
		{
			glfwTerminate();
			LOG_ERROR("GLFW could'nt create window");
			return false;
		}

		glfwSetKeyCallback(_window, OnInput);
		return true;
	}

	void Window::Update()
	{
		glfwPollEvents();
	}

	void Window::Render() const
	{
	}

	void Window::Destroy()
	{
		glfwDestroyWindow(_window);
		glfwTerminate();
		delete GetInstance();
	}

	const bool Window::WindowShouldClose() const
	{
		bool closeWindow = glfwWindowShouldClose(_window);
		if (closeWindow)
			LOG_INFO("Close window");
		return closeWindow;
	}

	void Window::DisplayError(int error, const char* description)
	{
		LOG_ERROR("GLFW [" + std::to_string(error) + "] " + description);
	}

	void Window::OnInput(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		const int CLOSE_APP_KEY = GLFW_KEY_ESCAPE;
		const int CLOSE_APP_ACTION = GLFW_PRESS;

		if (key == CLOSE_APP_KEY && action == CLOSE_APP_ACTION)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}
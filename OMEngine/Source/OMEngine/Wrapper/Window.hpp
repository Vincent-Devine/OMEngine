#pragma once

#include "OMEngine/Base.hpp"

class GLFWwindow;

namespace OM::Wrapper
{
	class OM_ENGINE_API Window
	{
	public:
		static Window* GetInstance();

		bool Initialisation();
		void Update();
		void Render() const;
		void Destroy();

		const bool WindowShouldClose() const;

	private:
		static inline Window* _instance = nullptr;

		GLFWwindow* _window;
		const unsigned int GLFW_MAJOR_VERSION = 4;
		const unsigned int GLFW_MINOR_VERSION = 5;

		static void DisplayError(int error, const char* description);
		static void OnInput(GLFWwindow* window, int key, int scancode, int action, int mods);
	};
}
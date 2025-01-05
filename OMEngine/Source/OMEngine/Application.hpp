#pragma once

#include "OMEngine/Base.hpp"
#include "OMEngine/Wrapper/Window.hpp"

namespace OM
{
	class OM_ENGINE_API Application
	{
	public:
		bool Initialisation();
		void Update();
		void Render() const;
		void Destroy();

		const bool WindowShouldClose() const;

	private:
		Wrapper::Window* _window;
	};
}


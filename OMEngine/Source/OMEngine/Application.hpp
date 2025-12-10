#pragma once

#include "OMEngine/Base.hpp"
#include "OMEngine/Platform/Window.hpp"
#include "OMEngine/Graphics/RHI/RHI.hpp"

namespace OM
{
	class OM_ENGINE_API Application
	{
	public:
		bool Initialize(HINSTANCE hInstance, int nCmdShow);
		void Update();
		void Render() const;
		void Destroy();

		const bool WindowShouldClose() const;

	private:
		Platform::Window* _window;
		Graphics::RHI::RHI* _rhi;
	};
}


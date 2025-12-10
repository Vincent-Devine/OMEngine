#pragma once

#include "OMEngine/Base.hpp"

namespace OM::Platform
{
	class OM_ENGINE_API Window
	{
	public:
		static Window* GetInstance();

		bool Initialisation(HINSTANCE hInstance, int nCmdShow);
		void Update();
		void Render() const;
		void Destroy();

		const bool GetWindowShouldClose() const { return _windowShouldClose; }
		HWND GetHWND() const { return _hwnd; }

	private:
		static inline Window* _instance = nullptr;

		HWND _hwnd; // Handle Window
		bool _windowShouldClose = false;

		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
}
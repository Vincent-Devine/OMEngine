#include "pch.h"

#include "OMEngine/Wrapper/Window.hpp"
#include <Windows.h>
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

	bool Window::Initialisation(HINSTANCE hInstance, int nCmdShow)
	{
		// Windows definition
		WNDCLASSEX windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = hInstance;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = L"MainWindow";
		RegisterClassExW(&windowClass);

		RECT windowRect = { 0, 0, static_cast<LONG>(OM::Utils::SCREEN_WIDTH), static_cast<LONG>(OM::Utils::SCREEN_HEIGHT) };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		WCHAR* windowName = {};
		MultiByteToWideChar(CP_ACP, 0, OM::Utils::GAME_NAME.c_str(), -1, windowName, 256);

		_hwnd = CreateWindow(
			windowClass.lpszClassName,
			windowName,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr,
			nullptr,
			hInstance,
			nullptr
		);

		ShowWindow(_hwnd, nCmdShow);

		return _hwnd != nullptr;
	}

	void Window::Update()
	{
		MSG msg = {};
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				_windowShouldClose = true;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void Window::Render() const
	{
	}

	void Window::Destroy()
	{
		delete GetInstance();
	}

	LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CREATE:
			return 0;

		case WM_KEYDOWN:
			return 0;

		case WM_KEYUP:
			return 0;

		case WM_PAINT:
			return 0;

		case WM_DESTROY:
			GetInstance()->_windowShouldClose = true;
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
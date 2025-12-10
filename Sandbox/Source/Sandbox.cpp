#include <Windows.h>
#include <iostream>
#include <fstream>

#include "OMEngine.hpp"

void RedirectIOToConsole()
{
    AllocConsole();

    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    std::ios::sync_with_stdio();

    std::cout.clear();
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();
}

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    RedirectIOToConsole(); // Open console for log

    // Initialisation
    OM::Application application;
    if (!application.Initialize(hInstance, nCmdShow))
    {
        application.Destroy();
        return 1;
    }

    // Game loop
    while (!application.WindowShouldClose())
    {
        application.Update();
        application.Render();
    }

    // Clean
    application.Destroy();
    return 0;
}

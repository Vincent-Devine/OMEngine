#include <Windows.h>

#include "OMEngine.hpp"

_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // Initialisation
    OM::Application application;
    if (!application.Initialisation(hInstance))
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

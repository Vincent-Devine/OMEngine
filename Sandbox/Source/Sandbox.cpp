#include "OMEngine.hpp"

int main()
{
    // Initialisation
    OM::Application application;
    if (!application.Initialisation())
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

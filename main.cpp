#include "Application.h"
#include <Windows.h>

int main()
{
    Application* app = new Application();

    app->init();
    app->run();

    return 0;
}

#include "Application.h"
#include <Windows.h>

/*
 * Author: Jatinder Bhachu
 */

int main()
{
    Application* app = new Application();

    app->init();
    app->run();

    return 0;
}

#include "application/Application.h"

#ifdef WIN32
#include <Windows.h>
#endif

int main(int argc, char* argv[]) {
#ifdef WIN32
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif
    Application app;
    app.start();

    return 0;
}
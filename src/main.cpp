#include "application/Application.h"

#ifdef WIN32
#include <Windows.h>
#endif

int main(int argc, char* argv[]) {
#ifdef WIN32
    //Hides console window if the project is built on Windows
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif
    Application app;
    app.run();

    return 0;
}
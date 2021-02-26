/*
 * Application
 * The purpose of this class is to setup the application side of this project as well as serve as an entry point
 * to the emulator.
 */

#pragma once

#include <SDL.h>
#include <string>
#include "RenderView.h"

class Application {
private:
    const static int DEFAULT_WINDOW_WIDTH = 160;
    const static int DEFAULT_WINDOW_HEIGHT = 144;
    const static std::string DEFAULT_WINDOW_CAPTION;
    // SDL related
    SDL_Window* window; // Application claims no ownership of this pointer.
    SDL_GLContext glContext;
    RenderView renderView;

    bool quit;

    void handleSDLEvents();
    void initSDL();
    void terminateSDL();

public:
    Application();
    void start();
};

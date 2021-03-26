/*
 * Application
 * The purpose of this class is to setup the application side of this project as well as serve as an entry point
 * to the emulator.
 */

#pragma once

#include <SDL.h>
#include <string>
#include <chrono> // time
#include <thread> // sleep

#include "IO/RenderView.h"
#include "gameboy/GameBoy.h"
#include "gameboy/Definitions.h"
#include "IO/AudioController.h"

class Application {
public:
    Application();
    void start();

private:
    const static std::string DEFAULT_WINDOW_CAPTION;

    SDL_Window* window;
    SDL_GLContext glContext;
    RenderView renderView;
    AudioController audio;
    GameBoy gameBoy;

    bool running;

    void init();
    void initSDL();
    void terminateSDL();
    void handleSDLEvents();
    void updateSDLWindowSize();
};

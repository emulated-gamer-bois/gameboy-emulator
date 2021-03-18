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
#include "imgui.h"
#include "RenderView.h"
#include "gameboy/GameBoy.h"
#include "gameboy/Definitions.h"
#include "Gui.h"
#define CONTROLLER_UP SDLK_w
#define CONTROLLER_DOWN SDLK_s
#define CONTROLLER_LEFT SDLK_a
#define CONTROLLER_RIGHT SDLK_d
#define CONTROLLER_A SDLK_f
#define CONTROLLER_B SDLK_g
#define CONTROLLER_START SDLK_h
#define CONTROLLER_SELECT SDLK_j
class Application {
public:
    Application();
    void start();

private:
    const static std::string DEFAULT_WINDOW_CAPTION;

    SDL_Window* window;
    SDL_GLContext glContext;
    RenderView renderView;
    GameBoy gameBoy;
    Gui gui;
    bool running;

    void init();
    void initSDL();
    void terminateSDL();
    void handleSDLEvents();
    void updateSDLWindowSize();
    void terminate();
};

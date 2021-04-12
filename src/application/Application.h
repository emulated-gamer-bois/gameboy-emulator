/*
 * application
 * The purpose of this class is to setup the application side of this project as well as serve as an entry point
 * to the emulator.
 */

#pragma once

#include <SDL.h>
#include <string>
#include <chrono> // time
#include <thread> // sleep
#include <cmath>
#include "../IO/RenderView.h"
#include "../gameboy/GameBoy.h"
#include "../gameboy/Definitions.h"
#include "../IO/AudioController.h"
#include "../helpers/ErrorReport.h"
#include "imgui.h"
#include "AppSettings.h"
#include "../GuiView.h"
#include "KeyBinds.h"
#include "../Controller.h"
#include "State.h"

class Application {
public:
    Application();
    void start();

private:
    int framesUntilStep;

    SDL_Window* window;
    SDL_GLContext glContext;
    int windowWidth;
    int windowHeight;

    State state;
    AppSettings settings;
    PaletteHandler paletteHandler;

    GameBoy gameBoy;
    AudioController audio;

    // Explicitly constructed in constructor.
    RenderView renderView;
    GuiView guiView;
    Controller controller;

    void initSDL();
    void terminate();

    void stepEmulation();
    void stepFast();
    void stepSlowly();
    void gameBoyStep();

    void correctWindowSize();
    void correctViewport();
};

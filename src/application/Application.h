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
#include "KeyBinds.h"
#include "../IO/GuiView.h"
#include "../IO/Controller.h"
#include "State.h"

class Application {
public:
    Application();
    /**
     * Main loop of application, runs until state is set as TERMINATION.
     * */
    void run();

private:
    int framesUntilStep;
    SDL_Window* window{};
    SDL_GLContext glContext{};
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
    /**
     * Steps the emulation depending on settings.emulationSpeedMultiplier.
     * */
    void stepEmulation();
    /**
     * StepFast steps the emulation settings.emulationSpeedMultiplier number of times.
     * Skips rendering and speeds up emulation only.
     * */
    void stepFast();
    /**
     * StepSlow renders 1/settings.emulationSpeedMultiplier frames before stepping the emulation once.
     * */
    void stepSlowly();
    /**
     * gameBoyStep steps the Game Boy once.
     * */
    void gameBoyStep();
    /**
    * Makes sure the window dimensions updates to match changes in RenderView dimensions.
    */
    void correctWindowSize();
    void correctViewport();
};

#pragma once

#include "AppSettings.h"
#include "State.h"
#include "../IO/RenderView.h"
#include "../IO/AudioController.h"
#include "../IO/GuiView.h"
#include "../IO/Controller.h"
#include "../gameboy/GameBoy.h"
/**
 * This class contains the main loop of the Emulator.
 */

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

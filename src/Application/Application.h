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
#include "../IO/RenderView.h"
#include "../gameboy/GameBoy.h"
#include "../gameboy/Definitions.h"
#include "../IO/AudioController.h"
#include "../helpers/ErrorReport.h"
#include "imgui.h"
#include "AppSettings.h"
#include "../Gui.h"
#include "Keybinds.h"
#include "../View.h"

class Application {
public:
    Application();
    void start();

private:
    enum State {
        EMULATION,
        MENU,
        TERMINATION
    } state;

    std::shared_ptr<AppSettings> settings{std::make_shared<AppSettings>()};
    AudioController audio;
    GameBoy gameBoy;
    View view = View(settings);
    float savedEmulationSpeed;
    int framesUntilStep{0};
    void init();
    void handleEmulatorInputPress(SDL_Keycode key);
    void handleEmulatorInputRelease(SDL_Keycode key);
    void terminate();
    void initSettings();
    void updateSound(uint8_t ready);

    void stepFast();

    void gameBoyStep();

    void stepSlowly();

    void stepEmulation();

    void handleSDLEvents();
};

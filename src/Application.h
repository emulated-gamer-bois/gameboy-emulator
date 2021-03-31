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
#include "imgui.h"
#include "AppSettings.h"
#include "Gui.h"
#include "Keybinds.h"

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
    SDL_Window* window;
    SDL_GLContext glContext;
    RenderView renderView;
    AudioController audio;
    GameBoy gameBoy;
    Gui gui = Gui(settings);

    float savedEmulationSpeed;

    void init();
    void initSDL();
    void terminateSDL();
    void handleSDLEvents();
    void handleEmulatorInput(SDL_Keycode key, int eventType);
    void updateSDLWindowSize();
    void terminate();
    void initSettings();

};

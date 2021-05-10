/**
 * Application delegates the sdl event handling to this class.
 */

#pragma once

#include <SDL.h>

#include "../application/State.h" // State
#include "../application/AppSettings.h" // Key binds
#include "../gameboy/GameBoy.h" // Communication with game boy joypad and joypad constants

#include "GuiView.h"

class Controller {
public:
    explicit Controller(AppSettings& settings, GuiView& guiView, GameBoy& gameBoy);

    /**
    * Handles SDL Events including keyboard input.
    * Acts differently depending on state of the application.
    * For example handling GUI or emulation.
     *
    * @param state state of the application.
    */
    State handleSDLEvents(State state);

private:
    void handleEmulatorInputPress(SDL_Keycode key);
    void handleEmulatorInputRelease(SDL_Keycode key);
    AppSettings& settings;
    GuiView& guiView;
    GameBoy& gameBoy;
    float savedEmulationSpeed;
};
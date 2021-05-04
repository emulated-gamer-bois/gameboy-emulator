

#ifndef LAME_BOY_CONTROLLER_H
#define LAME_BOY_CONTROLLER_H
#include <SDL.h>
#include <string>
#include <chrono> // time
#include <thread> // sleep
#include "../application/State.h" //State
#include "../application/AppSettings.h" //Keybinds
#include "../gameboy/Joypad.h" // Joypad
#include "../gameboy/GameBoy.h" // JOYPAD_Constants
#include "GuiView.h"

class Controller {
public:
    explicit Controller(AppSettings& settings, GuiView& guiView, GameBoy& gameBoy);
    /**
    * Handles SDL Events including keyboard input.
    * Acts differently depending on state of the application.
    * For example handling GUI or emulation.
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


#endif //LAME_BOY_CONTROLLER_H

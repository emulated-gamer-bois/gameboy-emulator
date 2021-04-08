//
// Created by isak_ on 2021-04-06.
//

#ifndef LAME_BOY_CONTROLLER_H
#define LAME_BOY_CONTROLLER_H
#include <SDL.h>
#include <string>
#include <chrono> // time
#include <thread> // sleep
#include "IO/RenderView.h"
#include "gameboy/GameBoy.h"
#include "gameboy/Definitions.h"
#include "IO/AudioController.h"
#include "helpers/ErrorReport.h"
#include "imgui.h"
#include "application/AppSettings.h"
#include "GuiView.h"
#include "application/Keybinds.h"
#include "application/State.h"

class Controller {
public:
    explicit Controller(AppSettings& settings, GuiView& guiView, GameBoy& gameBoy);
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

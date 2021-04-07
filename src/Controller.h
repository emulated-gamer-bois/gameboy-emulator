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
#include "Gui.h"
#include "application/Keybinds.h"
#include "View.h"
#include "application/State.h"

class Controller {
public:


    Controller(const std::shared_ptr<AppSettings> &settings, const std::shared_ptr<View> &view,const std::shared_ptr<GameBoy> &gameBoy);
    State handleSDLEvents(State state);

private:
    void handleEmulatorInputPress(SDL_Keycode key);
    void handleEmulatorInputRelease(SDL_Keycode key);
    std::shared_ptr<AppSettings> settings;
    std::shared_ptr<View> view;
    std::shared_ptr<GameBoy> gameBoy;
    float savedEmulationSpeed;

};


#endif //LAME_BOY_CONTROLLER_H

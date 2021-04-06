//
// Created by isak_ on 2021-04-06.
//

#ifndef LAME_BOY_VIEW_H
#define LAME_BOY_VIEW_H
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
#include "AppSettings.h"
#include "Gui.h"
#include "Keybinds.h"

class View {

    void RenderEmulation();
    void RenderGUI();
private:
    SDL_Window* window;
    SDL_GLContext glContext;
    RenderView renderView;
    AudioController audio;
    Gui gui = Gui(settings);

    void updateSDLWindowSize();

    void handleSDLEvents();

    void handleEmulatorInputPress(SDL_Keycode key);

    void handleEmulatorInputRelease(SDL_Keycode key);
};


#endif //LAME_BOY_VIEW_H

//
// Created by isaaklindgren on 2021-03-18.
//

#pragma once

#include <SDL.h>
#include <imgui.h>

#include "AppSettings.h"

class Gui {
public:
    Gui();
    void init(SDL_Window *window);
    void handleGui(SDL_Window *window, AppSettings* settings);
    void terminate(SDL_Window *window);
    void handleInput(SDL_Event event);

    void toggleToolbar();

private:
    ImGuiIO io;

    bool displayEditControls;
    bool displayFileDialog;
    bool displayToolbar;
    bool typing;

    void toolbar();

    void showEditControls();
    void showFileDialog(AppSettings* settings);
    void testkeyboard();
    int savekeybind();
};


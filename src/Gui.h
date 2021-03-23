//
// Created by isaaklindgren on 2021-03-18.
//

#pragma once

#include <SDL.h>
#include <imgui.h>
#include "Keybinds.h"

#include "AppSettings.h"

class Gui {
public:
    Gui(AppSettings * settings);
    void init(SDL_Window *window, SDL_GLContext *glContext,char *glsl_version);
    void handleGui(SDL_Window *window);
    void terminate(SDL_Window *window);
    void handleInput(SDL_Event event);
    AppSettings * settings;
    void toggleToolbar();

private:
    ImGuiIO io;
    void showEditControls();
    void keyBind();
    int keybindindex;
    void showKeyBind(const char *buttonName);

    bool displayEditControls;
    bool displayFileDialog;
    bool displayToolbar;
    bool typing;
    bool do_keybind;
    void toolbar();
    void showFileDialog();
};


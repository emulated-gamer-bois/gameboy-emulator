//
// Created by isaaklindgren on 2021-03-18.
//

#pragma once

#include <SDL.h>
#include <imgui.h>

#include "Keybinds.h"
#include "FileExplorer.h"
#include "AppSettings.h"

class Gui {
public:
    Gui(AppSettings * settings);
    void init(SDL_Window *window, SDL_GLContext *glContext,char *glsl_version);
    void handleGui(SDL_Window *window);
    void terminate();
    void handleInput(SDL_Event event);
    AppSettings * settings;
    void toggleToolbar();

private:
    // File dialog members
    FileExplorer fileExplorer;
    int selectedFile;

    // Key binding members
    int keyBindIndex;

    // File dialog window functions
    void showFileDialog();

    // Key binding window functions
    void showEditControls();
    void keyBind();

    bool displayEditControls;
    bool displayFileDialog;
    bool displayToolbar;
    bool waitingForKeyBind;
    void toolbar();
    const ImVec4 pressColor{ 0.0f, 0.217f, 1.0f, 0.784f };
    const ImVec4 releaseColor{ 0.202f, 0.549f, 0.798f, 0.784f };
    void disableWidgets();
    void displayPlaySpeed();
};


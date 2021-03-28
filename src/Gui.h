//
// Created by isaaklindgren on 2021-03-18.
//

#pragma once

#include <SDL.h>
#include <imgui.h>
#include <functional>

#include "Keybinds.h"
#include "FileExplorer.h"
#include "AppSettings.h"
#include "gameboy/Definitions.h"

class Gui {
public:
    Gui(AppSettings* settings);
    void init(SDL_Window *window, SDL_GLContext *glContext,char *glsl_version);
    void handleGui(SDL_Window *window);
    void handleInput(SDL_Event event);
    void terminate();

    // Enables/Disables the gui
    void toggleGui();

    void setLoadRomCallback(std::function<void(std::string)>&& loadRomCallback);

private:
    const ImVec4 pressColor{ 0.0f, 0.217f, 1.0f, 0.784f };
    const ImVec4 releaseColor{ 0.202f, 0.549f, 0.798f, 0.784f };

    AppSettings* settings;
    bool displayEditControls;
    bool displayFileDialog;
    bool displayToolbar;

    // File dialog members
    FileExplorer fileExplorer;
    std::function<void(std::string)> loadRomCallback;
    int selectedFile;

    // Key binding members
    int keyBindIndex;
    bool waitingForKeyBind;

    // File dialog window functions
    void showFileDialog();

    // Key binding window functions
    void showEditControls();
    void keyBind();

    // Speed settings functions
    void displayPlaySpeed();

    void showToolbar();
    void disableWidgets();

};


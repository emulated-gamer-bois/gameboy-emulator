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
#include "IO/PaletteHandler.h"
#include "helpers/ErrorReport.h"
#include "gameboy/Definitions.h"

class Gui {
public:
    Gui(std::shared_ptr<AppSettings> settings, std::shared_ptr<PaletteHandler> paletteHandler);
    void init(SDL_Window *window, SDL_GLContext *glContext,char *glsl_version);
    void handleGui(SDL_Window *window);
    void handleInput(SDL_Event event);
    void terminate();

    // Enables/Disables the gui
    void toggleGui();

    void setLoadRomCallback(std::function<void(std::string)>&& loadRomCallback);
    void setChangePaletteCallback(std::function<void(int)>&& changePaletteCallback);

private:
    // General gui constants
    const ImVec4 pressColor{ 0.0f, 0.217f, 1.0f, 0.784f };
    const ImVec4 releaseColor{ 0.202f, 0.549f, 0.798f, 0.784f };
    const float indentSpace = 20.f;
    const float listBoxWidth = 350.f;
    const float listBoxHeight = 150.f;
    const int windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;

    std::shared_ptr<AppSettings> settings;
    std::shared_ptr<PaletteHandler> paletteHandler;
    bool displayEditControls;
    bool displayFileDialog;
    bool displayPaletteSettings;
    bool displayToolbar;

    // File dialog members
    FileExplorer fileExplorer;
    std::function<void(std::string)> loadRomCallback;
    std::function<void(int)> changePaletteCallback;
    int selectedFile;

    // Key binding members
    int keyBindIndex;
    bool waitingForKeyBind;

    // Palette settings members
    int selectedPalette;

    // File dialog window functions
    void showFileDialog();

    // Key binding window functions
    void showEditControls();
    void keyBind();

    // Palette settings functions
    void showPaletteSettings();

    // Speed settings functions
    void displayPlaySpeed();

    void showToolbar();
    void disableWidgets();

};


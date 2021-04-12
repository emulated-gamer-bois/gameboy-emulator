//
// Created by isaaklindgren on 2021-03-18.
//

#pragma once

#include <SDL.h>
#include <imgui.h>
#include <functional>

#include "application/KeyBinds.h"
#include "FileExplorer.h"
#include "IO/PaletteHandler.h"
#include "helpers/ErrorReport.h"
#include "application/AppSettings.h"
#include "gameboy/Definitions.h"

class GuiView {
public:
    explicit GuiView(AppSettings& settings, PaletteHandler& paletteHandler);

    void updateAndRender(SDL_Window *window);
    void handleInput(SDL_Event event);
    void terminate();
    void initImGui(SDL_Window *window, SDL_GLContext *glContext, char *glsl_version);

    // Enables/Disables the gui
    void toggleGui();

    void setLoadRomCallback(std::function<void(std::string)>&& loadRomCallback);
    void setExitMenuCallback(std::function<void()>&& exitMenuCallback);
    void setExitProgramCallback(std::function<void()>&& exitProgramCallback);
    void setCorrectViewportCallback(std::function<void()>&& correctViewportCallback);
    void setChangeWindowSizeCallback(std::function<void(int width, int height)>&& changeWindowSizeCallback);
    void setGetWindowCenterCallback(std::function<void(int& x, int& y)>&& getWindowCenterCallback);

private:
    const ImVec4 pressColor{ 0.0f, 0.217f, 1.0f, 0.784f };
    const ImVec4 releaseColor{ 0.202f, 0.549f, 0.798f, 0.784f };
    const float indentSpace = 20.f;
    const float listBoxWidth = 350.f;
    const float listBoxHeight = 150.f;
    const int windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse;

    AppSettings& settings;
    PaletteHandler& paletteHandler;

    // GUI ------------------------------------------
    bool displayToolbar;
    void showToolbar();
    void disableWidgets();

    // Callbacks
    std::function<void(std::string)> loadRomCallback;
    std::function<void()> exitMenuCallback;
    std::function<void()> exitProgramCallback;
    std::function<void()> correctViewportCallback;
    std::function<void(int width, int height)> changeWindowSizeCallback;
    std::function<void(int& x, int& y)> getWindowCenterCallback;

    // File dialog ----------------------------------
    FileExplorer fileExplorer;
    int selectedFile;
    bool displayFileDialog;

    void showFileDialog();
    // Key binding ----------------------------------
    int keyBindIndex;
    bool waitingForKeyBind;
    bool displayEditControls;

    void showEditControls();
    void keyBind();

    // Palette settings -----------------------------
    int selectedPalette;
    int previewPalette;
    bool displayPaletteSettings;

    void showPaletteSettings();

    // Speed settings -------------------------------
    void generateEmulationSpeedItems();
    void generateWindowedSizeItems();

    void prepareCenteredWindow();
};


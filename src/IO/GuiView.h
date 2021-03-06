#pragma once

#include <SDL.h> // keypress
#include <imgui.h> // gui
#include <functional> // callback functions

#include "FileExplorer.h"
#include "PaletteHandler.h"
#include "../application/AppSettings.h" // KeyBinds and AppSettings
/**
 * A class that contains all ImGui code. Used for the emulators ui.
 */

class GuiView {
public:
    explicit GuiView(AppSettings& settings, PaletteHandler& paletteHandler);
    /**
     * Produces and renders new frames for the GUI.
     *
     * @param window window to render to.
     */
    void updateAndRender(SDL_Window *window);
    /**
     * Registers SDL_events with ImGui.
     *
     * @param event SDL_event to register.
     */
    void handleInput(SDL_Event event);
    /**
     * Terminates and shuts down all applications regarding the GUI.
     */
    void terminate();
    /**
     * Initializes ImGUI context and backend.
     *
     * @param window which window to use.
     * @param glContext which context to work within.
     * @param glsl_version version of OpenGL Shading Language.
     *
     */
    void initImGui(SDL_Window *window, SDL_GLContext *glContext, char *glsl_version);

    /**
     * Toggles whether or not to display the GUI.
     */
    void toggleGui();

    /**
     * Used to define action upon interaction with LoadRom-button.
     * @param loadRomCallback function to be called.
     */
    void setLoadRomCallback(std::function<void(std::string)>&& loadRomCallback);
    /**
     * Used to define action upon interaction with ExitMenu-button.
     * @param exitMenuCallback function to be called.
     */
    void setExitMenuCallback(std::function<void()>&& exitMenuCallback);
    /**
     * Used to define action upon interaction with ExitProgram-button.
     * @param exitProgramCallback function to be called.
     */
    void setExitProgramCallback(std::function<void()>&& exitProgramCallback);
    /**
     * Used to define action upon interaction with correctViewport-button (keep aspect ratio).
     * @param correctViewportCallback function to be called.
     */
    void setCorrectViewportCallback(std::function<void()>&& correctViewportCallback);
    /**
     * Used to define action to change window size, making it fullscreen or otherwise scale it.
     * @param changeWindowSizeCallback function to be called.
     */
    void setChangeWindowSizeCallback(std::function<void(int width, int height)>&& changeWindowSizeCallback);
    /**
     * Used to define action to, upon opening a new window, center it.
     * @param getWindowCenterCallback function to be called.
     */
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

    // Volume settings ------------------------------
    bool displayVolumeSettings;

    void showVolumeSettings();

    // Other ----------------------------------------
    void generateEmulationSpeedItems();
    void generateWindowedSizeItems();

    void prepareCenteredWindow();
};


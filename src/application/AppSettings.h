#pragma once

#include <string>

#include "KeyBinds.h"
/**
 * This class acts as a container for data that is being shared between Application, RenderView, GuiView,
 * AudioController and Controller. The purpose is to have one place where the settings for the application can be
 * stored and modified. It also defines the file structure of the .settings file which provides a way to modify
 * settings outside of the application.
 */
class AppSettings {
public:
    // File path settings
    std::string romPath;

    // Emulation settings
    KeyBinds keyBinds;
    float emulationSpeedMultiplier;

    // Screen settings
    int windowedWidth;
    int windowedHeight;
    bool fullscreen;
    bool keepAspectRatio;
    int paletteNumber;

    // Audio
    float masterVolume;

    AppSettings();
    ~AppSettings();

private:
    inline static const std::string romFolderKey = "romFolder";

    inline static const std::string kbAKey = "keyBind_a";
    inline static const std::string kbBKey = "keyBind_b";
    inline static const std::string kbStartKey = "keyBind_start";
    inline static const std::string kbSelectKey = "keyBind_select";
    inline static const std::string kbLeftKey = "keyBind_left";
    inline static const std::string kbRightKey = "keyBind_right";
    inline static const std::string kbUpKey = "keyBind_up";
    inline static const std::string kbDownKey = "keyBind_down";
    inline static const std::string kbTurboKey = "keyBind_turbo";

    inline static const std::string windowWidthKey = "windowWidth";
    inline static const std::string windowHeightKey = "windowedHeight";

    inline static const std::string fullscreenKey = "fullscreen";
    inline static const std::string keepAspectRatioKey = "keepAspectRatio";
    inline static const std::string paletteNumberKey = "paletteNumber";

    inline static const std::string masterVolumeKey = "masterVolume";

    void loadSettings();
    void saveSettings();
};
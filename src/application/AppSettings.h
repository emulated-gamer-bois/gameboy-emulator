/*
 * AppSettings
 * In the AppSettings struct we will put all emulator settings. Settings that will be changed by ImGui, read from file
 * and written to file.
 */

#pragma once

#include <string>
#include <fstream>
#include <regex>

#include "KeyBinds.h"
#include "../helpers/ErrorReport.h"
#include "../gameboy/Definitions.h"

class AppSettings {
public:
    /**
     *The path where the file dialog will always be in when opened.
     **/
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
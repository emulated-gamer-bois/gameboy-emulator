/*
 * AppSettings
 * In the AppSettings struct we will put all emulator settings. Settings that will be changed by ImGui, read from file
 * and written to file.
 */

#pragma once

#include <string>

#include "KeyBinds.h"

#include "../gameboy/Definitions.h"

class AppSettings {
public:
    std::string romPath; // The path where the file dialog will always be in when opened.

    // Emulation settings
    KeyBinds keyBinds;
    float emulationSpeedMultiplier;

    // Screen settings
    int windowedWidth;
    int windowedHeight;
    bool fullscreen;
    bool keepAspectRatio;
    int paletteNumber;

    AppSettings();
    ~AppSettings();
};
/*
 * AppSettings
 * In the AppSettings struct we will put all emulator settings. Settings that will be changed by ImGui, read from file
 * and written to file.
 */

#pragma once
#include "Keybinds.h"
#include <string>



struct AppSettings {
    std::string defaultPath; // The path where the file dialog will always be in when opened.
    Keybinds keyBinds;
    int screenMultiplier;
};
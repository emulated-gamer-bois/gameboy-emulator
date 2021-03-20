/*
 * AppSettings
 * In the AppSettings struct we will put all emulator settings. Settings that will be changed by ImGui, read from file
 * and written to file.
 */

#pragma once

#include <string>

struct KeyBindings {
    int keyA;
    int keyB;
    int keyStart;
    int keySelect;
    int keyLeft;
    int keyRight;
    int keyUp;
    int keyDown;
};

struct AppSettings {
    std::string defaultPath; // The path where the file dialog will always be in when opened.
    KeyBindings keyBindings;
    int screenMultiplier;
};
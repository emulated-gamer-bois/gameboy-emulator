#include "AppSettings.h"

AppSettings::AppSettings() {
    // Try to load from file
    // bool loaded = loadSettings();

    // If no file was found.
    // if (!loaded) {
    romPath = "..";
    keyBinds.init_keybinds();
    screenMultiplier = 4;
    paletteNumber = 0;
    emulationSpeedMultiplier = 1.f; // Not saved or loaded from file
    // }
}

AppSettings::~AppSettings() {
    // When AppSettings goes out of scope, save to settings.txt
    // saveSettings();
}
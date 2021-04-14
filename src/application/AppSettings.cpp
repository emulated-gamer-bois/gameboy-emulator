#include "AppSettings.h"

AppSettings::AppSettings():
        romPath{".."}, emulationSpeedMultiplier{1.f}, windowedWidth{LCD_WIDTH * MIN_WINDOW_SIZE_MULTIPLIER},
        windowedHeight{LCD_HEIGHT * MIN_WINDOW_SIZE_MULTIPLIER}, fullscreen{false}, keepAspectRatio{true},
        paletteNumber{0}, masterVolume{0.75f}
{
    // Try to load from file
    // loadSettings();
}

AppSettings::~AppSettings() {
    // When AppSettings goes out of scope, save to settings.txt
    // saveSettings();
}
#include "AppSettings.h"

#include <fstream>
#include <regex>

#include "../IO/PaletteHandler.h" // paletteAmount constant
#include "../helpers/ErrorReport.h"
#include "../gameboy/Definitions.h"

AppSettings::AppSettings():
        romPath{".."}, emulationSpeedMultiplier{1.f}, windowedWidth{LCD_WIDTH * MIN_WINDOW_SIZE_MULTIPLIER},
        windowedHeight{LCD_HEIGHT * MIN_WINDOW_SIZE_MULTIPLIER}, fullscreen{false}, keepAspectRatio{true},
        paletteNumber{0}, masterVolume{0.25f}
{
    // Try to load from file
    loadSettings();

}

AppSettings::~AppSettings() {
    saveSettings();
}

void AppSettings::loadSettings() {
    std::ifstream file;
    file.open(".settings");

    if (file.is_open()) {
        const int lineLength = 256;
        char line[lineLength];

        while (file.getline(line, lineLength)) {
            std::string lineStr(line);

            // Ignore empty lines
            if (lineStr.empty()) {
                continue;
            }

            // Ignore comments.
            if (lineStr.at(0) == '%') {
                continue;
            }

            // Ignore rows without key/value pair.
            int equalSignIndex = lineStr.find('=');
            if (equalSignIndex == -1) {
                continue;
            }

            // Ignore rows where key or value is empty
            std::string key(lineStr.substr(0, equalSignIndex));
            std::string value(lineStr.substr(equalSignIndex + 1));
            if (key.empty() || value.empty()) {
                continue;
            }

            // if string value expected
            if (key == romFolderKey) {
                romPath = value;
                continue;
            }

            // if int or bool value expected
            std::regex naturalOnly("([0-9]+)");
            if (std::regex_match(value, naturalOnly)) {
                int naturalValue = std::stoi(value);

                bool valueIsValidKeyCode = (SDL_GetScancodeFromKey(naturalValue) >= 0
                        && SDL_GetScancodeFromKey(naturalValue) <= SDL_NUM_SCANCODES);

                if (key == kbAKey && valueIsValidKeyCode) {
                    keyBinds.a.keyVal = naturalValue;
                    keyBinds.a.keyBind = SDL_GetKeyName(naturalValue);
                } else if (key == kbBKey && valueIsValidKeyCode) {
                    keyBinds.b.keyVal = naturalValue;
                    keyBinds.b.keyBind = SDL_GetKeyName(naturalValue);
                } else if (key == kbStartKey && valueIsValidKeyCode) {
                    keyBinds.start.keyVal = naturalValue;
                    keyBinds.start.keyBind = SDL_GetKeyName(naturalValue);
                } else if (key == kbSelectKey && valueIsValidKeyCode) {
                    keyBinds.select.keyVal = naturalValue;
                    keyBinds.select.keyBind = SDL_GetKeyName(naturalValue);
                } else if (key == kbLeftKey && valueIsValidKeyCode) {
                    keyBinds.left.keyVal = naturalValue;
                    keyBinds.left.keyBind = SDL_GetKeyName(naturalValue);
                } else if (key == kbRightKey && valueIsValidKeyCode) {
                    keyBinds.right.keyVal = naturalValue;
                    keyBinds.right.keyBind = SDL_GetKeyName(naturalValue);
                } else if (key == kbUpKey && valueIsValidKeyCode) {
                    keyBinds.up.keyVal = naturalValue;
                    keyBinds.up.keyBind = SDL_GetKeyName(naturalValue);
                } else if (key == kbDownKey && valueIsValidKeyCode) {
                    keyBinds.down.keyVal = naturalValue;
                    keyBinds.down.keyBind = SDL_GetKeyName(naturalValue);
                } else if (key == kbTurboKey && valueIsValidKeyCode) {
                    keyBinds.turboMode.keyVal = naturalValue;
                    keyBinds.turboMode.keyBind = SDL_GetKeyName(naturalValue);
                } else if (key == windowWidthKey && (naturalValue >= LCD_WIDTH * MIN_WINDOW_SIZE_MULTIPLIER)) {
                    windowedWidth = naturalValue;
                } else if (key == windowHeightKey && (naturalValue >= LCD_HEIGHT * MIN_WINDOW_SIZE_MULTIPLIER)) {
                    windowedHeight = naturalValue;
                } else if (key == fullscreenKey) {
                    fullscreen = naturalValue;
                } else if (key == keepAspectRatioKey) {
                    keepAspectRatio = naturalValue;
                } else if (key == paletteNumberKey && naturalValue >= 0 && naturalValue < PaletteHandler::paletteAmount) {
                    paletteNumber = naturalValue;
                }
                continue;
            }

            // Float value expected
            std::regex floatOnly("([0-9]*.[0-9]+|[0-9]+)");
            if (std::regex_match(value, floatOnly)) {
                float floatValue = std::stof(value);
                if (key == masterVolumeKey && floatValue >= 0.f && floatValue <= 1.f) {
                        masterVolume = floatValue;
                }
            }
        }
        file.close();
    }
}

void AppSettings::saveSettings() {
    std::ofstream file;
    file.open(".settings", std::fstream::trunc);

    if (file.is_open()) {
        file << "% This is an auto generated settings file. Every row that does not follow the" << std::endl;
        file << "% key=value (where key or value is not empty) will be ignored. The same goes" << std::endl;
        file << "% comments such as these three first lines." << std::endl;
        file << std::endl;
        file << "% Will be ignored if this is not a valid file path." << std::endl;
        file << romFolderKey << "=" << romPath << std::endl;
        file << std::endl;
        file << "% Lookup SDL keycodes to modify these values manually." << std::endl;
        file << kbAKey << "=" << keyBinds.a.keyVal << std::endl;
        file << kbBKey << "=" << keyBinds.b.keyVal << std::endl;
        file << kbStartKey << "=" << keyBinds.start.keyVal << std::endl;
        file << kbSelectKey << "=" << keyBinds.select.keyVal << std::endl;
        file << kbLeftKey << "=" << keyBinds.left.keyVal << std::endl;
        file << kbRightKey << "=" << keyBinds.right.keyVal << std::endl;
        file << kbUpKey << "=" << keyBinds.up.keyVal << std::endl;
        file << kbDownKey << "=" << keyBinds.down.keyVal << std::endl;
        file << kbTurboKey << "=" << keyBinds.turboMode.keyVal << std::endl;
        file << std::endl;
        file << "% Must be larger than or equal to " << LCD_WIDTH * MIN_WINDOW_SIZE_MULTIPLIER << std::endl;
        file << windowWidthKey << "=" << windowedWidth << std::endl;
        file << "% Must be larger than or equal to " << LCD_HEIGHT * MIN_WINDOW_SIZE_MULTIPLIER << std::endl;
        file << windowHeightKey << "=" << windowedHeight << std::endl;
        file << std::endl;
        file << "% These are boolean values (0 or 1). " << std::endl;
        file << fullscreenKey << "=" << fullscreen << std::endl;
        file << keepAspectRatioKey << "=" << keepAspectRatio << std::endl;
        file << std::endl;
        file << "% Must be between 0 and " << PALETTE_AMOUNT << "." << std::endl;
        file << paletteNumberKey << "=" << paletteNumber << std::endl;
        file << std::endl;
        file << "% Must be between 0 and 1 inclusive." << std::endl;
        file << masterVolumeKey << "=" << masterVolume;

        file.close();
    } else {
        NON_FATAL_ERROR("Settings could not be saved.");
    }
}

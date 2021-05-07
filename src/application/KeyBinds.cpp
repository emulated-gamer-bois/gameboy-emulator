#include "KeyBinds.h"
KeyBinds::KeyBinds():
    keyBinds({&a, &b, &start, &select, &left, &right, &up, &down, &turboMode}), nonMappableKeys({SDLK_ESCAPE})
{
    a.keyval = SDLK_j;
    b.keyval = SDLK_h;
    start.keyval= SDLK_g;
    select.keyval = SDLK_f;
    left.keyval = SDLK_a;
    right.keyval = SDLK_d;
    up.keyval = SDLK_w;
    down.keyval = SDLK_s;
    turboMode.keyval = SDLK_SPACE;

    for(int i=0; i < keyBinds.capacity(); i++){
        keyBinds[i]->keybind = SDL_GetKeyName(keyBinds[i]->keyval);
    }

    a.action_description = "A";
    b.action_description = "B";
    start.action_description = "Start";
    select.action_description = "Select";
    left.action_description = "Gamepad Left";
    right.action_description = "Gamepad Right";
    up.action_description = "Gamepad Up";
    down.action_description = "Gamepad Down";
    turboMode.action_description = "Turbo Mode";
}

bool KeyBinds::editKeyBinds(const bool keysDown[], int keyBindIndex) {
    for(int i = 0; i < 512; i++) {
        if (keysDown[i]) {
            if(SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(i)) == SDLK_ESCAPE) {
                return true;
            }
            if (validKey(keyBindIndex,static_cast<SDL_Scancode>(i))) {
                keyBinds[keyBindIndex]->keybind = SDL_GetScancodeName(static_cast<SDL_Scancode>(i));
                keyBinds[keyBindIndex]->keyval = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(i));
                return true;
            }
        }
    }

    return false;
}

bool KeyBinds::validKey(int keyBindIndex, SDL_Scancode scanCode) {
    bool forbiddenKeyBind = false;

    for (int i = 0; i < keyBinds.capacity(); i++) {
        if (i != keyBindIndex) {
            forbiddenKeyBind |= (keyBinds[i]->keyval == SDL_GetKeyFromScancode(scanCode));
        }
    }

    for (int i = 0; i < nonMappableKeys.capacity(); i++) {
        forbiddenKeyBind |= (SDL_GetKeyFromScancode(scanCode) == nonMappableKeys[i]);
    }

    return !forbiddenKeyBind;
}
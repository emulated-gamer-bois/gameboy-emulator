#include "KeyBinds.h"
KeyBinds::KeyBinds():
    keyBinds({&a, &b, &start, &select, &left, &right, &up, &down, &turboMode}), nonMappableKeys({SDLK_ESCAPE})
{
    a.keyVal = SDLK_j;
    b.keyVal = SDLK_h;
    start.keyVal= SDLK_g;
    select.keyVal = SDLK_f;
    left.keyVal = SDLK_a;
    right.keyVal = SDLK_d;
    up.keyVal = SDLK_w;
    down.keyVal = SDLK_s;
    turboMode.keyVal = SDLK_SPACE;

    for(int i=0; i < keyBinds.capacity(); i++){
        keyBinds[i]->keyBind = SDL_GetKeyName(keyBinds[i]->keyVal);
    }

    a.actionDescription = "A";
    b.actionDescription = "B";
    start.actionDescription = "Start";
    select.actionDescription = "Select";
    left.actionDescription = "Gamepad Left";
    right.actionDescription = "Gamepad Right";
    up.actionDescription = "Gamepad Up";
    down.actionDescription = "Gamepad Down";
    turboMode.actionDescription = "Turbo Mode";
}

bool KeyBinds::editKeyBinds(const bool keysDown[], int keyBindIndex) {
    for(int i = 0; i < 512; i++) {
        if (keysDown[i]) {
            if(SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(i)) == SDLK_ESCAPE) {
                return true;
            }
            if (validKey(keyBindIndex,static_cast<SDL_Scancode>(i))) {
                keyBinds[keyBindIndex]->keyBind = SDL_GetScancodeName(static_cast<SDL_Scancode>(i));
                keyBinds[keyBindIndex]->keyVal = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(i));
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
            forbiddenKeyBind |= (keyBinds[i]->keyVal == SDL_GetKeyFromScancode(scanCode));
        }
    }

    for (int i = 0; i < nonMappableKeys.capacity(); i++) {
        forbiddenKeyBind |= (SDL_GetKeyFromScancode(scanCode) == nonMappableKeys[i]);
    }

    return !forbiddenKeyBind;
}
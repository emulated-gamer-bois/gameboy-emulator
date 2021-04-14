//
// Created by isaaklindgren on 2021-03-19.
//

#include "KeyBinds.h"


KeyBinds::KeyBinds() {
    a.keyval = SDLK_j;
    b.keyval = SDLK_h;
    start.keyval= SDLK_g;
    select.keyval = SDLK_f;
    left.keyval = SDLK_a;
    right.keyval = SDLK_d;
    up.keyval = SDLK_w;
    down.keyval = SDLK_s;
    turboMode.keyval = SDLK_SPACE;
    for(int i=0; i < keybinds.capacity(); i++){
        this->keybinds[i]->keybind = SDL_GetKeyName(
                this->keybinds[i]->keyval);
    }
    a.action_description = "A";
    b.action_description = "B";
    start.action_description = "Start";
    select.action_description = "Select";
    left.action_description = "Gamepad Left";
    right.action_description = "Gamepad Right";
    up.action_description = "Gamepad Up";
    down.action_description = "Gamepad Down";
    start.action_description = "Start";
    turboMode.action_description = "Turbo Mode";
}

bool KeyBinds::editKeyBinds(bool keysDown [], int keyBindIndex) {

    for(int i =0;i<512;i++) {
        if (keysDown[i]) {
            if(SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(i)) == SDLK_ESCAPE){
                return true;
            }
            if (validKey(keyBindIndex,static_cast<SDL_Scancode>(i))) {
                keybinds[keyBindIndex]->keybind = SDL_GetScancodeName(
                        static_cast<SDL_Scancode>(i));
                keybinds[keyBindIndex]->keyval = SDL_GetKeyFromScancode(
                        static_cast<SDL_Scancode>(i));
            return true;
            }
        }
    }
    return false;
}
bool KeyBinds::validKey(int keyBindIndex, SDL_Scancode scanCode) {
    bool forbiddenKeyBind = false;
    for (int j = 0; j < keybinds.capacity(); j++) {
        if (j != keyBindIndex)
            forbiddenKeyBind |= keybinds[j]->keyval == SDL_GetKeyFromScancode(scanCode);
    }
    for(int j=0;j<forbiddenKeys.capacity();j++){
        forbiddenKeyBind |= SDL_GetKeyFromScancode(scanCode) == forbiddenKeys[j];
    }
    return !forbiddenKeyBind;
}

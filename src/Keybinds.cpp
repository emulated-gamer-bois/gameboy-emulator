//
// Created by isaaklindgren on 2021-03-19.
//

#include "Keybinds.h"


void Keybinds::init_keybinds() {
    a.keyval = SDLK_j;
    b.keyval = SDLK_h;
    start.keyval= SDLK_g;
    select.keyval = SDLK_f;
    left.keyval = SDLK_a;
    right.keyval = SDLK_d;
    up.keyval = SDLK_w;
    down.keyval = SDLK_s;
    doubleSpeed.keyval = SDLK_SPACE;
    for(int i=0;i<controllerButtons.capacity();i++){
        this->controllerButtons[i]->keybind = SDL_GetKeyName(
                this->controllerButtons[i]->keyval);
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
    doubleSpeed.action_description = "Hold for double speed";
}

bool Keybinds::editKeyBinds( bool keysDown [], int keyBindIndex) {

    for(int i =0;i<512;i++) {
        if (keysDown[i]) {
            if(SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(i)) == SDLK_ESCAPE){
                return true;
            }
            if (validKey(keyBindIndex,static_cast<SDL_Scancode>(i))) {
                controllerButtons[keyBindIndex]->keybind = SDL_GetScancodeName(
                        static_cast<SDL_Scancode>(i));
                controllerButtons[keyBindIndex]->keyval = SDL_GetKeyFromScancode(
                        static_cast<SDL_Scancode>(i));
            return true;
            }
        }
    }
    return false;
}
bool Keybinds::validKey(int keyBindIndex,SDL_Scancode scanCode) {
    bool forbiddenKeyBind = false;
    for (int j = 0; j < controllerButtons.capacity(); j++) {
        if (j != keyBindIndex)
            forbiddenKeyBind |= controllerButtons[j]->keyval == SDL_GetKeyFromScancode(scanCode);
    }
    for(int j=0;j<forbiddenKeys.capacity();j++){
        forbiddenKeyBind |= SDL_GetKeyFromScancode(scanCode) == forbiddenKeys[j];
    }
    return !forbiddenKeyBind;
}

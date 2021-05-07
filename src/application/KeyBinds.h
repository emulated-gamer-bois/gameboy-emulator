#pragma once

#include <vector>
#include <SDL.h>
#include <string>

class KeyBinds {
public:
    /**
     * Struct containing:
     * action_description - text used for describing the specific action, such as "Start".
     * keybind - text used to describe which key is used for this keybind, for example "Left Ctrl"
     * keyval  - SDL key value.
     * */
    struct action {
        std::string action_description;
        std::string keybind;
        int keyval;
    };

    action a;
    action b;
    action start;
    action select;
    action left;
    action right;
    action up;
    action down;
    action turboMode;
    std::vector<action*> keyBinds;

    KeyBinds();
    /**
     * Edits keybinds. Checks which key has been pressed of keysDown[] and maps this new keybind to
     * the key located at keyBindIndex in the vector keyBinds.
    * */
    bool editKeyBinds(const bool keysDown[], int keyBindIndex);


private:
    /**
     * Contains all keys which the user is prevented to bind any actions to.
     * */
    std::vector<int> nonMappableKeys;
    /**
     * Checks whether or not a keybind is a valid keybind or not.
     * Is done by checking that it is not already bound as well as not being one of the "nonMappable"-keys.
     *
     * */
    bool validKey(int keyBindIndex, SDL_Scancode scanCode);
};
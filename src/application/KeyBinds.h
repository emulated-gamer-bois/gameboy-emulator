#pragma once

#include <SDL.h>
#include <vector>
#include <string>
/**
 * This class is closely related to the AppSettings class since this also is a container for application settings.
 * It makes it possible to remap keys.
 */
class KeyBinds {
public:
    /**
     * Struct containing:
     * actionDescription - text used for describing the specific action, such as "Start".
     * keyBind - text used to describe which key is used for this keybind, for example "Left Ctrl"
     * keyVal  - SDL key value.
     * */
    struct action {
        std::string actionDescription;
        std::string keyBind;
        int keyVal;
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
     *
     * @param keysDown array containing status of all keys, pressed or not.
     * @param keyBindIndex which index in keyBinds-vector to be edited.
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
     * @param keyBindIndex which index in keyBinds-vector to check.
     * @param scanCode which key to edit check if it is valid or not to bind a new action to.
     * */
    bool validKey(int keyBindIndex, SDL_Scancode scanCode);
};
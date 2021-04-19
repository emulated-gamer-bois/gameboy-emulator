#pragma once

#include <vector>
#include <string>
#include <SDL.h>

class KeyBinds {
public:
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

    bool editKeyBinds(const bool keysDown[], int keyBindIndex);


private:
    std::vector<int> nonMapableKeys;

    bool validKey(int keyBindIndex, SDL_Scancode scanCode);
};
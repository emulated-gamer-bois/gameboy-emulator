//
// Created by isaaklindgren on 2021-03-19.
//

#ifndef LAME_BOY_KEYBINDS_H
#define LAME_BOY_KEYBINDS_H

#include <vector>
#include "string"
#include <SDL.h>



class Keybinds {
public:
    void init_keybinds();
    struct controller_button {
        std::string action_description;
        std::string keybind;
        int keyval;
    };
    controller_button a,b,start,select,left,right,up,down,doubleSpeed;
    std::vector<controller_button*> controllerButtons = {&a,&b,&start,&select,&left,&right,&up,&down,&doubleSpeed};
    bool editKeyBinds(bool *keysDown, int keyBindIndex);


private:
    std::vector<int> forbiddenKeys {SDLK_ESCAPE};
    bool validKey(int keyBindIndex,SDL_Scancode scanCode);
};


#endif //LAME_BOY_KEYBINDS_H

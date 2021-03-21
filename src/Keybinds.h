//
// Created by isaaklindgren on 2021-03-19.
//

#ifndef LAME_BOY_KEYBINDS_H
#define LAME_BOY_KEYBINDS_H
#include "string"



class Keybinds {
public:
    int nControllers=0;
    void init_keybinds();
    struct controller_button {
        std::string action_description;
        std::string keybind;
        int keyval;
    };
    controller_button a,b,start,select,left,right,up,down;
    controller_button * controllerButtons [8] = {&a,&b,&start,&select,&left,&right,&up,&down};
private:

};


#endif //LAME_BOY_KEYBINDS_H

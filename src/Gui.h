//
// Created by isaaklindgren on 2021-03-18.
//

#ifndef LAME_BOY_GUI_H
#define LAME_BOY_GUI_H


#include <SDL.h>

class Gui {



public:
    void gui(SDL_Window *window);

    void init(SDL_Window *window);
    void terminate(SDL_Window *window);
};


#endif //LAME_BOY_GUI_H

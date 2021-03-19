//
// Created by isaaklindgren on 2021-03-18.
//

#ifndef LAME_BOY_GUI_H
#define LAME_BOY_GUI_H


#include <SDL.h>
#include <imgui.h>


class Gui {



public:
    void draw_gui(SDL_Window *window);
    void init(SDL_Window *window);
    void terminate(SDL_Window *window);
    void handleInput(SDL_Event event);

private:
    ImGuiIO io;
    void showEditControls(bool &show);
    void testkeyboard();
    int savekeybind();
};


#endif //LAME_BOY_GUI_H

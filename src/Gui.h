//
// Created by isaaklindgren on 2021-03-18.
//

#ifndef LAME_BOY_GUI_H
#define LAME_BOY_GUI_H


#include <SDL.h>
#include <imgui.h>
#include "Keybinds.h"


class Gui {



public:
    Gui(Keybinds * controller);
    void draw_gui(SDL_Window *window);
    void init(SDL_Window *window);
    void terminate(SDL_Window *window);
    void handleInput(SDL_Event event);
    Keybinds* controller;

private:
    ImGuiIO io;
    void showEditControls();
    void keyBind();
    int keybindindex;
    void showKeyBind(const char *buttonName);
};


#endif //LAME_BOY_GUI_H

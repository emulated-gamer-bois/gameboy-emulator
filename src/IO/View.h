//
// Created by isak_ on 2021-04-06.
//

#ifndef LAME_BOY_VIEW_H
#define LAME_BOY_VIEW_H


#include <memory>
#include <functional>
#include "../application/AppSettings.h"
#include "RenderView.h"
#include "Gui.h"

class View {
public:
    View(std::shared_ptr<AppSettings> sharedPtr);
    void View::init(std::function<void(std::string)> &&loadRomCallback,int screenMultiplier);
    void render(bool renderEmu, uint8_t *textureData);
    void renderGui();
    void terminateView();
    void swapbuffers();
    void toggleGui();
    void handleGuiInput(SDL_Event event);

private:
    SDL_Window* window;
    SDL_GLContext glContext;
    RenderView renderView;
    std::shared_ptr<AppSettings> settings;
    Gui gui;
    void updateSDLWindowSize();
    void initSDL();
    void renderEmulation(uint8_t *textureData);
    void terminateSDL();


};


#endif //LAME_BOY_VIEW_H

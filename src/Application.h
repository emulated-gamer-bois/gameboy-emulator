/*
 * Application
 * The purpose of this class is to setup the application side of this project as well as serve as an entry point
 * to the emulator.
 */

#ifndef LAME_BOY_APPLICATION_H
#define LAME_BOY_APPLICATION_H

#include <SDL.h>
#include <string>
#include <GL/glew.h>

class Application {
private:
    const static int DEFAULT_WINDOW_WIDTH = 160;
    const static int DEFAULT_WINDOW_HEIGHT = 144;
    const static std::string DEFAULT_WINDOW_CAPTION;

    //SDL_Window* window;
    SDL_GLContext glContext;

    //void initAppliction();
public:
    void run();
    void initAppliction();
    void destroyAppliction();
    SDL_Window* window;
};

#endif //LAME_BOY_APPLICATION_H

//#ifdef _WIN32
//extern "C" _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
//#endif

#include <chrono> //
#include <iostream>

#include "Application.h"
#include "RenderView.h"
#include "temp_screen_helper.h"



int main(int argc, char* argv[]) {
    Application app;
    app.start();

    return 0;
}

void Application::start() {
    initSDL();

    RenderView rv(1);

    bool stopRendering = false;
    auto startTime = std::chrono::system_clock::now();
    float currentTime = 0.0f;
    float previousTime = 0.0f;
    float deltaTime = 0.0f;

    // Deluxe temp------------------------------------------------------------------------------------------------------
    // Manual memory management, no exception handling
    uint8_t* byteArr = temporary::importTempscreen();
    if (!byteArr) {
        terminateSDL();
        return;
    }
    rv.setScreenTexture(byteArr);
    delete[] byteArr;
    // End Deluxe temp--------------------------------------------------------------------------------------------------

    while(!stopRendering) {
        //update currentTime
        std::chrono::duration<float> timeSinceStart = std::chrono::system_clock::now() - startTime;
        previousTime = currentTime;
        currentTime = timeSinceStart.count();
        deltaTime = currentTime - previousTime;
        // render to window
        rv.render();

        // Swap front and back buffer. This frame will now been displayed.
        SDL_GL_SwapWindow(window);

        // Check if it is time to quit or not
        {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    stopRendering = true;
                }
            }
        }
    }

    terminateSDL();
}

const std::string Application::DEFAULT_WINDOW_CAPTION = "Lame Boy";


void Application::initSDL() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "%s: %s\n", "Couldn't initialize SDL", SDL_GetError());
        return;
    }
    atexit(SDL_Quit);
    SDL_GL_LoadLibrary(nullptr); // Load the default OpenGL library

    // Request an OpenGL 4.1 context and require hardware acceleration.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // Try to put OpenGL in debug mode.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    // Use double buffering. May be on by default. Not sure.
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Create the window.
    window = SDL_CreateWindow(DEFAULT_WINDOW_CAPTION.c_str(),
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          DEFAULT_WINDOW_WIDTH,
                                          DEFAULT_WINDOW_HEIGHT,
                                          SDL_WINDOW_OPENGL);

    if(!window) {
        fprintf(stderr, "%s: %s\n", "Couldn't set video mode", SDL_GetError());
        return;
    }

    // Get gl context and set it to the current context for this window.
    glContext = SDL_GL_CreateContext(window);
    if(!glContext) {
        fprintf(stderr, "%s: %s\n", "Failed to create OpenGL context", SDL_GetError());
        return;
    }

    // Don't know if this is needed.
    glewInit();

    // Enable v-sync.
    SDL_GL_SetSwapInterval(1);

    // Workaround for AMD. Must not be removed.
    if(!glBindFragDataLocation) {
        glBindFragDataLocation = glBindFragDataLocationEXT;
    }
}

void Application::terminateSDL() {
    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();
}
// Uncomment if opengl won't work
/*
#ifdef _WIN32
extern "C" _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
#endif
*/

#include <chrono> // std::chrono::system_clock::now()

#include "Application.h" // Implements
#include "RenderView.h"

// TEMP
#include <fstream> // file reading
void TEMP_setTexture(const char* filename, RenderView& rv);
// END TEMP

const std::string Application::DEFAULT_WINDOW_CAPTION = "Lame Boy";

/*
 * This function initializes sdl, creates a window and gets a OpenGL context. If an error occurs in this function
 * the program should exit.
 */
void Application::initSDL() {
    // Initialize SDL and check if SDL could be initialize.
    assert(SDL_Init(SDL_INIT_VIDEO) >= 0);
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
    // Couldn't set video mode.
    assert(window);

    // Get gl context and set it to the current context for this window.
    glContext = SDL_GL_CreateContext(window);
    // Failed to create OpenGL context.
    assert(glContext);

    // Don't know if this is needed.
    glewInit();

    // Enable v-sync.
    SDL_GL_SetSwapInterval(1);

    // Workaround for AMD. Must not be removed.
    if(!glBindFragDataLocation) {
        glBindFragDataLocation = glBindFragDataLocationEXT;
    }
}

/*
 * Cleans up after SDL.
 */
void Application::terminateSDL() {
    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();
}

/*
 * Call from main. Is the main loop of this application.
 */
void Application::start() {
    initSDL();

    RenderView renderView;

    bool quit = false;
    auto startTime = std::chrono::system_clock::now();
    float currentTime = 0.0f;
    float previousTime = 0.0f;
    float deltaTime = 0.0f;

    TEMP_setTexture("../src/title.pixdata", renderView);

    while(!quit) {
        //update currentTime
        std::chrono::duration<float> timeSinceStart = std::chrono::system_clock::now() - startTime;
        previousTime = currentTime;
        currentTime = timeSinceStart.count();
        deltaTime = currentTime - previousTime;

        // Resize window to the size of the render view if needed.
        {
            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            if (w != renderView.getWidth() || h != renderView.getHeight()) {
                SDL_SetWindowSize(window, renderView.getWidth(), renderView.getHeight());
            }
        }

        // render to window
        renderView.render();

        // Swap front and back buffer. This frame will now been displayed.
        SDL_GL_SwapWindow(window);

        // Check if it is time to quit or not
        {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_QUIT:
                        quit = true;
                        break;
                    case SDL_KEYDOWN:
                        switch( event.key.keysym.sym ){
                            case SDLK_1:
                                renderView.setPalette(PALETTE_POCKET);
                                break;
                            case SDLK_2:
                                renderView.setPalette(PALETTE_DMG);
                                break;
                            case SDLK_3:
                                renderView.setPalette(PALETTE_DMG_SMOOTH);
                                break;
                            case SDLK_4:
                                break;
                            default:
                                break;
                        }
                }
            }
        }
    }

    terminateSDL();
}

/////////////////////////////////////////////////////////////////////////
////                           - TEMPORARY -                         ////
/////////////////////////////////////////////////////////////////////////
// Manual memory management, no exception handling and global scope.
void TEMP_setTexture(const char* filename, RenderView& rv) {
    int pixelAmount = 23040;
    char* rgbPixels = new char[pixelAmount * 3];

    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (file.is_open()) {
        file.read(rgbPixels, pixelAmount * 3);
        file.close();
    }
    else {
        delete[] rgbPixels;
        return;
    }

    auto* redPixels = new uint8_t[pixelAmount];
    for (int i = 0; i < pixelAmount; i += 1) {
        redPixels[i] = rgbPixels[i * 3];
    }

    rv.setScreenTexture(redPixels);

    delete[] rgbPixels;
    delete[] redPixels;
}
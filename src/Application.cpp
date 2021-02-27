// Uncomment if opengl won't work
/*
#ifdef _WIN32
extern "C" _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
#endif
*/

#include <chrono> // std::chrono::system_clock::now()
#include <thread> // sleep

#include "Application.h" // Implements
#include "RenderView.h"
#include "gameboy/GameBoy.h"

#define CONTROLLER_UP SDLK_w
#define CONTROLLER_DOWN SDLK_s
#define CONTROLLER_LEFT SDLK_a
#define CONTROLLER_RIGHT SDLK_d
#define CONTROLLER_A SDLK_f
#define CONTROLLER_B SDLK_g
#define CONTROLLER_START SDLK_h
#define CONTROLLER_SELECT SDLK_j

// TEMP
#include <fstream> // file reading
void TEMP_setTexture(const char* filename, RenderView& rv);
// END TEMP

const std::string Application::DEFAULT_WINDOW_CAPTION = "Lame Boy";

Application::Application() {
    running = true;
}

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
    renderView.initGL();

    float fps = 60.0f;
    float frameTime = 1000 / fps;

    auto startTime = std::chrono::system_clock::now();
    std::chrono::duration<float> currentTime = startTime - startTime;
    std::chrono::duration<float> previousTime;
    std::chrono::duration<float> elapsedTime;

    TEMP_setTexture("../src/title.pixdata", renderView);

    this->gameBoy.load_boot_rom("../roms/boot_infinite_loop.bin");
    // gameboy->load_game_rom("../roms/game.gb");

    // gameboy->cpu_dump();

    while(running) {
        previousTime = currentTime;

        do {
            this->handleSDLEvents();
            this->gameBoy.step();
        } while (!this->gameBoy.readyToDraw());

        // Resize window to the size of the render view if needed.
        this->updateSDLWindowSize();
        // render to window
        //this->renderView.setScreenTexture(this->gameBoy.getScreen());
        this->renderView.render();

        // Swap front and back buffer. This frame will now been displayed.
        SDL_GL_SwapWindow(window);

        //update currentTime
        currentTime = std::chrono::system_clock::now() - startTime;
        elapsedTime = currentTime - previousTime;

        //std::cout << "------------------------------" << std::endl;
        //std::cout << elapsedTime.count() << std::endl;

        if (elapsedTime.count() < frameTime) {
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(frameTime - elapsedTime.count())));
        }

    }

    terminateSDL();
}

void Application::handleSDLEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                switch( event.key.keysym.sym ){
                    case CONTROLLER_LEFT:
                        gameBoy.joypad_input(JOYPAD_LEFT, JOYPAD_PRESS);
                        renderView.setPalette(PALETTE_DMG_SMOOTH);
                        break;
                    case CONTROLLER_RIGHT:
                        gameBoy.joypad_input(JOYPAD_RIGHT, JOYPAD_PRESS);
                        renderView.setPalette(PALETTE_DMG_SMOOTH);
                        break;
                    case CONTROLLER_UP:
                        gameBoy.joypad_input(JOYPAD_UP, JOYPAD_PRESS);
                        renderView.setPalette(PALETTE_DMG_SMOOTH);
                        break;
                    case CONTROLLER_DOWN:
                        gameBoy.joypad_input(JOYPAD_DOWN, JOYPAD_PRESS);
                        renderView.setPalette(PALETTE_DMG_SMOOTH);
                        break;
                    case CONTROLLER_A:
                        gameBoy.joypad_input(JOYPAD_A, JOYPAD_PRESS);
                        renderView.setPalette(PALETTE_DMG_SMOOTH);
                        break;
                    case CONTROLLER_B:
                        gameBoy.joypad_input(JOYPAD_B, JOYPAD_PRESS);
                        renderView.setPalette(PALETTE_DMG_SMOOTH);
                        break;
                    case CONTROLLER_START:
                        gameBoy.joypad_input(JOYPAD_START, JOYPAD_PRESS);
                        renderView.setPalette(PALETTE_DMG_SMOOTH);
                        break;
                    case CONTROLLER_SELECT:
                        gameBoy.joypad_input(JOYPAD_SELECT, JOYPAD_PRESS);
                        renderView.setPalette(PALETTE_DMG_SMOOTH);
                        break;
                }
                break;
            case SDL_KEYUP:
                switch( event.key.keysym.sym ){
                    case CONTROLLER_LEFT:
                        gameBoy.joypad_input(JOYPAD_LEFT, JOYPAD_RELEASE);
                        renderView.setPalette(PALETTE_DMG);
                        break;
                    case CONTROLLER_RIGHT:
                        gameBoy.joypad_input(JOYPAD_RIGHT, JOYPAD_RELEASE);
                        renderView.setPalette(PALETTE_DMG);
                        break;
                    case CONTROLLER_UP:
                        gameBoy.joypad_input(JOYPAD_UP, JOYPAD_RELEASE);
                        renderView.setPalette(PALETTE_DMG);
                        break;
                    case CONTROLLER_DOWN:
                        gameBoy.joypad_input(JOYPAD_DOWN, JOYPAD_RELEASE);
                        renderView.setPalette(PALETTE_DMG);
                        break;
                    case CONTROLLER_A:
                        gameBoy.joypad_input(JOYPAD_A, JOYPAD_RELEASE);
                        renderView.setPalette(PALETTE_DMG_SMOOTH);
                        break;
                    case CONTROLLER_B:
                        gameBoy.joypad_input(JOYPAD_B, JOYPAD_RELEASE);
                        renderView.setPalette(PALETTE_DMG_SMOOTH);
                        break;
                    case CONTROLLER_START:
                        gameBoy.joypad_input(JOYPAD_START, JOYPAD_RELEASE);
                        renderView.setPalette(PALETTE_DMG_SMOOTH);
                        break;
                    case CONTROLLER_SELECT:
                        gameBoy.joypad_input(JOYPAD_SELECT, JOYPAD_RELEASE);
                        renderView.setPalette(PALETTE_DMG_SMOOTH);
                        break;
                }
        }
    }
}

void Application::updateSDLWindowSize() {
    int width;
    int height;
    SDL_GetWindowSize(window, &width, &height);
    if (width != renderView.getWidth() || height != renderView.getHeight()) {
        SDL_SetWindowSize(window, renderView.getWidth(), renderView.getHeight());
    }
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
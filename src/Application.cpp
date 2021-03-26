// Uncomment if opengl won't work
/*
#ifdef _WIN32
extern "C" _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
#endif
*/

#include "Application.h"
#include "AppTimer.h"

#define CONTROLLER_UP SDLK_w
#define CONTROLLER_DOWN SDLK_s
#define CONTROLLER_LEFT SDLK_a
#define CONTROLLER_RIGHT SDLK_d
#define CONTROLLER_A SDLK_f
#define CONTROLLER_B SDLK_g
#define CONTROLLER_START SDLK_h
#define CONTROLLER_SELECT SDLK_j

// TEMP ----------------------------------------------------------------------------------------------------------------
#include <fstream>
void TEMP_setTexture(const char* filename, RenderView& rv);
// END TEMP ------------------------------------------------------------------------------------------------------------

const std::string Application::DEFAULT_WINDOW_CAPTION = "Lame Boy";

/**
 * Constructor
 */
Application::Application() {
    this->running = true;
}

/**
 * The main loop of this Application. It works like this:
 *     1. Initialize application and loop.
 *     2. Handle events and step the emulator until the ppu have left VBLANK.
 *     3. Prepare for rendering and render a frame.
 *     4. Wait for timing purposes
 */
void Application::start() {
    this->init();

    float frameTime = 1000.f / LCD_REFRESH_RATE;

    AppTimer timer;

    while(running) {
        // Create time stamp.
        timer.tick();

        // Update emulation
        while (!this->gameBoy.isReadyToDraw() && !this->gameBoy.isReadyToPlaySound()) {
            this->handleSDLEvents();
            this->gameBoy.step();
        }

        if(this->gameBoy.isReadyToPlaySound()) {
            //Play audio
            //this->audio.playBuffers(this->gameBoy.getAudioBuffers());

            auto state = this->gameBoy.getState();
            this->audio.stopSource(0);
            if(state->enable_square_a) {
                this->audio.playGBSquare(0, state->duty_square_a, state->frequency_square_a);
            }

            this->audio.stopSource(2);
            if(state->enable_wave) {
                this->audio.playGBWave(2, state->waveform_wave, state->frequency_wave);
            }
            gameBoy.confirmPlay();
        }

        if(this->gameBoy.isReadyToDraw()) {
            // Prepare for rendering, render and swap buffer.
            this->updateSDLWindowSize();
            this->renderView.setScreenTexture(this->gameBoy.getScreenTexture().get());
            this->renderView.render();
            SDL_GL_SwapWindow(this->window);
            this->gameBoy.confirmDraw();

            // Time application to 60Hz
            float msSinceTick = timer.msSinceTick();
            if (msSinceTick < frameTime) {
                int msToSleep = frameTime - msSinceTick;
                std::this_thread::sleep_for(std::chrono::milliseconds(msToSleep));
            }
        }
    }

    terminateSDL();
}

void Application::init() {
    this->initSDL();
    this->renderView.initGL();
    this->audio.init();

    // TEMP ------------------------------------------------------------------------------------------------------------
    this->gameBoy.load_rom("../roms/gb/boot_lameboy_big.gb", "../roms/games/not_tetris.gb");
    // END TEMP --------------------------------------------------------------------------------------------------------
}

/*
 * This function initializes SDL, creates a window and gets a OpenGL context. If an error occurs in this function
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
    this->window = SDL_CreateWindow(DEFAULT_WINDOW_CAPTION.c_str(),
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          LCD_WIDTH,
                                          LCD_HEIGHT,
                                          SDL_WINDOW_OPENGL);
    assert(this->window);

    // Get gl context and set it to the current context for this window.
    this->glContext = SDL_GL_CreateContext(this->window);
    assert(this->glContext);

    // Don't know if this is needed.
    glewInit();

    // Disable v-sync. The gameboy lcd is 60Hz and that will be synced manually.
    SDL_GL_SetSwapInterval(0);

    // Workaround for AMD. Must not be removed.
    if(!glBindFragDataLocation) {
        glBindFragDataLocation = glBindFragDataLocationEXT;
    }
}

/**
 * Cleans up after SDL.
 */
void Application::terminateSDL() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/**
 * Handles SDL Events including keyboard input.
 */
void Application::handleSDLEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                this->running = false;
                break;
            case SDL_KEYDOWN:
                switch( event.key.keysym.sym ){
                    case CONTROLLER_LEFT:
                        this->gameBoy.joypad_input(JOYPAD_LEFT, JOYPAD_PRESS);
                        break;
                    case CONTROLLER_RIGHT:
                        this->gameBoy.joypad_input(JOYPAD_RIGHT, JOYPAD_PRESS);
                        break;
                    case CONTROLLER_UP:
                        this->gameBoy.joypad_input(JOYPAD_UP, JOYPAD_PRESS);
                        break;
                    case CONTROLLER_DOWN:
                        this->gameBoy.joypad_input(JOYPAD_DOWN, JOYPAD_PRESS);
                        break;
                    case CONTROLLER_A:
                        this->gameBoy.joypad_input(JOYPAD_A, JOYPAD_PRESS);
                        break;
                    case CONTROLLER_B:
                        this->gameBoy.joypad_input(JOYPAD_B, JOYPAD_PRESS);
                        break;
                    case CONTROLLER_START:
                        this->gameBoy.joypad_input(JOYPAD_START, JOYPAD_PRESS);
                        break;
                    case CONTROLLER_SELECT:
                        this->gameBoy.joypad_input(JOYPAD_SELECT, JOYPAD_PRESS);
                        break;
                    // CHOOSE PALETTE
                    case SDLK_1:
                        this->renderView.setPalette(PALETTE_DMG_SMOOTH);
                        break;
                    case SDLK_2:
                        this->renderView.setPalette(PALETTE_DMG);
                        break;
                    case SDLK_3:
                        this->renderView.setPalette(PALETTE_POCKET);
                        break;
                }
                break;
            case SDL_KEYUP:
                switch( event.key.keysym.sym ){
                    case CONTROLLER_LEFT:
                        this->gameBoy.joypad_input(JOYPAD_LEFT, JOYPAD_RELEASE);
                        break;
                    case CONTROLLER_RIGHT:
                        this->gameBoy.joypad_input(JOYPAD_RIGHT, JOYPAD_RELEASE);
                        break;
                    case CONTROLLER_UP:
                        this->gameBoy.joypad_input(JOYPAD_UP, JOYPAD_RELEASE);
                        break;
                    case CONTROLLER_DOWN:
                        this->gameBoy.joypad_input(JOYPAD_DOWN, JOYPAD_RELEASE);
                        break;
                    case CONTROLLER_A:
                        this->gameBoy.joypad_input(JOYPAD_A, JOYPAD_RELEASE);
                        break;
                    case CONTROLLER_B:
                        this->gameBoy.joypad_input(JOYPAD_B, JOYPAD_RELEASE);
                        break;
                    case CONTROLLER_START:
                        this->gameBoy.joypad_input(JOYPAD_START, JOYPAD_RELEASE);
                        break;
                    case CONTROLLER_SELECT:
                        this->gameBoy.joypad_input(JOYPAD_SELECT, JOYPAD_RELEASE);
                        break;
                }
                break;
        }
    }
}

/**
 * Makes sure the window dimensions updates to match changes in RenderView dimensions.
 */
void Application::updateSDLWindowSize() {
    int width;
    int height;
    SDL_GetWindowSize(this->window, &width, &height);
    if (width != this->renderView.getWidth() || height != this->renderView.getHeight()) {
        SDL_SetWindowSize(this->window, this->renderView.getWidth(), this->renderView.getHeight());
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

// Uncomment if opengl won't work
/*
#ifdef _WIN32
extern "C" _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
#endif
*/

#include "Application.h"
#include "Timer.h"
#include "Gui.h"

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

    Timer timer;

    while (running) {
        // Create time stamp.
        timer.tick();

        // Update emulation
        while (!this->gameBoy.isReadyToDraw()) {
            this->handleSDLEvents();
            this->gameBoy.step();
        }
        // Prepare for rendering, render and swap buffer.
        this->updateSDLWindowSize();
        this->renderView.setScreenTexture(this->gameBoy.getScreenTexture().get());
        this->renderView.render();
        //TODO Can add some bools or stuff to enable or disable draw_gui, this is just temporary.
        this->gui.draw_gui(this->window);
        SDL_GL_SwapWindow(this->window);
        this->gameBoy.confirmDraw();

        // Time application to 60Hz
        float msSinceTick = timer.msSinceTick();
        if (msSinceTick < frameTime) {
            int msToSleep = frameTime - msSinceTick;
            std::this_thread::sleep_for(std::chrono::milliseconds(msToSleep));
        }
    }
    terminate();
}

void Application::init() {
    this->initSDL();
    this->init_controller();
    this->renderView.initGL();
    this->gui.init(window);

    // TEMP ------------------------------------------------------------------------------------------------------------
    this->gameBoy.load_rom("../roms/gb/boot_lameboy_big.gb", "../roms/instr_timing/instr_timing.gb");
    // END TEMP --------------------------------------------------------------------------------------------------------
}

void Application::terminate() {
    this->gui.draw_gui(this->window);
    terminateSDL();
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
    if (!glBindFragDataLocation) {
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
        SDL_Keycode sym = event.key.keysym.sym;
        gui.handleInput(event);
        if (event.type == SDL_QUIT) {
            this->running = false;
            break;
        }
        if (event.type == SDL_KEYDOWN) {
            if (sym == IO_left) {
                this->gameBoy.joypad_input(JOYPAD_LEFT, JOYPAD_PRESS);
                this->renderView.setPalette(PALETTE_DMG_SMOOTH);
                break;
            }
            if (sym == IO_right) {
                this->gameBoy.joypad_input(JOYPAD_RIGHT, JOYPAD_PRESS);
                this->renderView.setPalette(PALETTE_DMG_SMOOTH);
                break;
            }
            if (sym == IO_up) {
                this->gameBoy.joypad_input(JOYPAD_UP, JOYPAD_PRESS);
                this->renderView.setPalette(PALETTE_DMG_SMOOTH);
                break;
            }
            if (sym == IO_down) {
                this->gameBoy.joypad_input(JOYPAD_DOWN, JOYPAD_PRESS);
                this->renderView.setPalette(PALETTE_DMG_SMOOTH);
                break;
            }
            if (sym == IO_a) {
                this->gameBoy.joypad_input(JOYPAD_A, JOYPAD_PRESS);
                this->renderView.setPalette(PALETTE_DMG_SMOOTH);
                break;
            }
            if (sym == IO_b) {
                this->gameBoy.joypad_input(JOYPAD_B, JOYPAD_PRESS);
                this->renderView.setPalette(PALETTE_DMG_SMOOTH);
                break;
            }
            if (sym == IO_b) {
                this->gameBoy.joypad_input(JOYPAD_B, JOYPAD_PRESS);
                this->renderView.setPalette(PALETTE_DMG_SMOOTH);
                break;
            }
            if (sym == IO_start) {
                this->gameBoy.joypad_input(JOYPAD_START, JOYPAD_PRESS);
                this->renderView.setPalette(PALETTE_DMG_SMOOTH);
                break;
            }
            if (sym == IO_select) {
                this->gameBoy.joypad_input(JOYPAD_SELECT, JOYPAD_PRESS);
                this->renderView.setPalette(PALETTE_DMG_SMOOTH);
                break;
            }
        }
        if (event.type == SDL_KEYUP) {
            if (sym == IO_left) {
                this->gameBoy.joypad_input(JOYPAD_LEFT, JOYPAD_RELEASE);
                this->renderView.setPalette(PALETTE_DMG);
                break;
            }
            if (sym == IO_right) {
                this->gameBoy.joypad_input(JOYPAD_RIGHT, JOYPAD_RELEASE);
                this->renderView.setPalette(PALETTE_DMG);
                break;
            }
            if (sym == IO_up) {
                this->gameBoy.joypad_input(JOYPAD_UP, JOYPAD_RELEASE);
                this->renderView.setPalette(PALETTE_DMG);
                break;
            }
            if (sym == IO_down) {
                this->gameBoy.joypad_input(JOYPAD_DOWN, JOYPAD_RELEASE);
                this->renderView.setPalette(PALETTE_DMG);
                break;
            }
            if (sym == IO_a) {
                this->gameBoy.joypad_input(JOYPAD_A, JOYPAD_RELEASE);
                this->renderView.setPalette(PALETTE_DMG_SMOOTH);
                break;
            }
            if (sym == IO_b) {
                this->gameBoy.joypad_input(JOYPAD_B, JOYPAD_RELEASE);
                this->renderView.setPalette(PALETTE_DMG_SMOOTH);
                break;
            }
            if (sym == IO_b) {
                this->gameBoy.joypad_input(JOYPAD_B, JOYPAD_PRESS);
                this->renderView.setPalette(PALETTE_DMG_SMOOTH);
                break;
            }
            if (sym == IO_start) {
                this->gameBoy.joypad_input(JOYPAD_START, JOYPAD_RELEASE);
                this->renderView.setPalette(PALETTE_DMG_SMOOTH);
                break;
            }
            if (sym == IO_select) {
                this->gameBoy.joypad_input(JOYPAD_SELECT, JOYPAD_RELEASE);
                this->renderView.setPalette(PALETTE_DMG_SMOOTH);
                break;
            }
        }

        break;

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


void Application::init_controller() {
    //TODO load settings from file.
    IO_a = 102;
    IO_b = 103;
    IO_start = 104;
    IO_select = 106;
    IO_left = 97;
    IO_right = 100;
    IO_up = 119;
    IO_down = 115;
}



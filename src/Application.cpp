// Uncomment if opengl won't work
/*
#ifdef _WIN32
extern "C" _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
#endif
*/

#include "Application.h"
#include "AppTimer.h"

/**
 * Constructor
 */
Application::Application() {
    state = State::MENU;
    initSettings();
    savedEmulationSpeed = settings.emulationSpeedMultiplier;
    renderView.setScreenMultiplier(settings.screenMultiplier);
}

/**
 * The main loop of this Application. It works like this:
 *     1. Initialize application and loop.
 *     2. Handle events and step the emulator until the ppu have left VBLANK.
 *     3. Prepare for rendering and render a frame.
 *     4. Wait for timing purposes
 */
void Application::start() {
    init();
    float frameTime = 1000.f / LCD_REFRESH_RATE;
    AppTimer timer;
    while (state != State::TERMINATION) {
        // Create time stamp.
        timer.tick();
        handleSDLEvents();

        // Step through emulation until playspeed number of frames are produced, then display the last one.
        if (state == State::EMULATION && gameBoy.isOn()) {
            for (int i = 0; i < settings.emulationSpeedMultiplier; i++) {
                if (gameBoy.isReadyToDraw()) {
                    //Actually discards frame until settings.playSpeed number of frames have been produced.
                    gameBoy.confirmDraw();
                }
                while (!gameBoy.isReadyToDraw()) {
                    gameBoy.step();
                }
            }
            renderView.setScreenTexture(gameBoy.getScreenTexture().get());
            // TODO: find a better way to handle texture fetching than needing to call gameBoy.confirmDraw()
            gameBoy.confirmDraw();
        }

        updateSDLWindowSize();
        renderView.render();

        if (state == State::MENU) {
            gui.handleGui(window);
        }

        SDL_GL_SwapWindow(window);

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
    initSDL();
    renderView.initGL();
    gui.init(window, &glContext, "#version 130"); // GLSL version

    // Delegate actual loading of roms to the gui class.
    gui.setLoadRomCallback([this](std::string&& romPath) -> void {
        gameBoy.load_rom("../roms/gb/boot_lameboy_big.gb", romPath);
        state = State::EMULATION;
        gui.toggleGui();
    });
}

void Application::terminate() {
    gui.terminate();
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
    window = SDL_CreateWindow(EMULATOR_NAME_STRING,
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              LCD_WIDTH,
                              LCD_HEIGHT,
                              SDL_WINDOW_OPENGL);
    assert(window);

    // Get gl context and set it to the current context for this window.
    glContext = SDL_GL_CreateContext(window);
    assert(glContext);

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
        SDL_Keycode key = event.key.keysym.sym;

        if (state == State::MENU) {
            gui.handleInput(event);
        }

        switch (event.type) {
            case SDL_QUIT:
                state = State::TERMINATION;
                break;

            case SDL_KEYDOWN:
                // Disable key repeat
                if (event.key.repeat != 0) {
                    break;
                }

                if (key == SDLK_ESCAPE) {
                    gui.toggleGui();
                    state = (state == State::EMULATION) ? State::MENU : State::EMULATION;
                }
                if (key == SDLK_SPACE) {
                    savedEmulationSpeed = settings.emulationSpeedMultiplier;
                    settings.emulationSpeedMultiplier = MAX_EMULATION_SPEED_FLOAT;
                }
                if (state == State::EMULATION) {
                    handleEmulatorInput(key, JOYPAD_PRESS);
                }
                break;

            case SDL_KEYUP:
                if (key == SDLK_SPACE) {
                    settings.emulationSpeedMultiplier = savedEmulationSpeed;
                }
                if (state == State::EMULATION) {
                    handleEmulatorInput(key, JOYPAD_RELEASE);
                }
                break;
        }
    }
}

void Application::handleEmulatorInput(SDL_Keycode key, int eventType) {
    if (key == settings.keyBinds.left.keyval) {
        gameBoy.joypad_input(JOYPAD_LEFT, eventType);
    } else if (key == settings.keyBinds.right.keyval) {
        gameBoy.joypad_input(JOYPAD_RIGHT, eventType);
    } else if (key == settings.keyBinds.up.keyval) {
        gameBoy.joypad_input(JOYPAD_UP, eventType);
    } else if (key == settings.keyBinds.down.keyval) {
        gameBoy.joypad_input(JOYPAD_DOWN, eventType);
    } else if (key == settings.keyBinds.a.keyval) {
        gameBoy.joypad_input(JOYPAD_A, eventType);
    } else if (key == settings.keyBinds.b.keyval) {
        gameBoy.joypad_input(JOYPAD_B, eventType);
    } else if (key == settings.keyBinds.start.keyval) {
        gameBoy.joypad_input(JOYPAD_START, eventType);
    } else if (key == settings.keyBinds.select.keyval) {
        gameBoy.joypad_input(JOYPAD_SELECT, eventType);
    }
}

/**
 * Makes sure the window dimensions updates to match changes in RenderView dimensions.
 */
void Application::updateSDLWindowSize() {
    int width;
    int height;
    SDL_GetWindowSize(window, &width, &height);
    if (width != renderView.getWidth() || height != renderView.getHeight()) {
        SDL_SetWindowSize(window, renderView.getWidth(), renderView.getHeight());
    }
}

/**
 * Initialize settings to some default values. This should be called if settings couldn't be loaded from file.
 */
void Application::initSettings() {
    settings.keyBinds.init_keybinds();
    settings.screenMultiplier = 4;
    settings.romPath = "..";
    settings.emulationSpeedMultiplier = 1;
}



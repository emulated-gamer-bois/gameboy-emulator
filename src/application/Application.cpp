// Uncomment if opengl won't work
/*
#ifdef _WIN32
extern "C" _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
#endif
*/

#include "Application.h"
#include "../helpers/AppTimer.h"
#include "../helpers/ErrorReport.h"
#include "Game-Boy-FL.cpp"
#include <SDL.h>
#include <string>
#include <chrono> // time
#include <thread> // sleep
#include <cmath>
Application::Application():
    framesUntilStep{0}, windowWidth{settings.windowedWidth}, windowHeight{settings.windowedHeight},
    state{State::MENU}, audio(settings), renderView(settings, paletteHandler),
    guiView(settings, paletteHandler), controller(settings, guiView, gameBoy)
{
    initSDL(); // Creates gl context and sdl window. This needs to be called before other inits.
    renderView.initGL();
    guiView.initImGui(window, &glContext, "#version 130");
    correctViewport();

    guiView.setLoadRomCallback([this](std::string&& romPath) -> void {
        gameBoy.loadRom("../roms/gb/boot_lameboy_big.gb", romPath);
    });

    guiView.setExitMenuCallback([this]() -> void {
        state = State::EMULATION;
        guiView.toggleGui();
    });

    guiView.setExitProgramCallback([this]() -> void {
        state = State::TERMINATION;
    });

    guiView.setCorrectViewportCallback([this]() -> void {
        correctViewport();
    });

    guiView.setChangeWindowSizeCallback([this](int width, int height) -> void {
        SDL_SetWindowSize(window, width, height);
    });

    guiView.setGetWindowCenterCallback([this](int& x, int& y) -> void {
       x = windowWidth * 0.5f;
       y = windowHeight * 0.5f;
    });
}


void Application::run() {
    AppTimer timer;
    float frameTime = 1000.f / LCD_REFRESH_RATE;

    while (state != State::TERMINATION) {
        // Create time stamp.
        timer.tick();

        // Resize window if requested
        correctWindowSize();

        // Clear renderView
        renderView.clear();
        // Step through emulation until playspeed number of frames are produced, then display the last one.
        if (state == State::EMULATION && gameBoy.isOn()) {
            stepEmulation();
            if (gameBoy.isReadyToDraw()) {
                gameBoy.confirmDraw();
            }
            renderView.setScreenTexture(gameBoy.getScreenTexture().get());
        }
        renderView.render();

        this->state = controller.handleSDLEvents(state);
        // Render menu
        if (state == State::MENU) {
            audio.stopSound();
            guiView.updateAndRender(window);
        }
        SDL_GL_SwapWindow(window);

        // Time application to 60Hz
        float msSinceTick = timer.msSinceTick();
        if (msSinceTick < frameTime) {
            int msToSleep = frameTime - msSinceTick;
            std::this_thread::sleep_for(std::chrono::milliseconds(msToSleep));
        }
    }

    if (gameBoy.save()) {
        std::cout << "Saved successfully" << std::endl;
    } else {
        std::cout << "Save failed" << std::endl;
    }

    terminate();
}

void Application::initSDL() {
// Initialize SDL and check if SDL could be initialize.
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        FATAL_ERROR("SDL failed to initialize");
    }
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
                              settings.windowedWidth,
                              settings.windowedHeight,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {FATAL_ERROR("Failed to create SDL window."); }

    SDL_Surface *surface;
    surface = SDL_CreateRGBSurfaceFrom(
            gimp_image.pixel_data,
            gimp_image.width,
            gimp_image.height,
            32,
            gimp_image.bytes_per_pixel*gimp_image.width,
            0x00ff0000,
            0x0000ff00,
            0x000000ff,
            0xff000000);
    SDL_SetWindowIcon(window, surface);
    SDL_FreeSurface(surface);

    SDL_SetWindowMinimumSize(window,
                             LCD_WIDTH * MIN_WINDOW_SIZE_MULTIPLIER,
                             LCD_HEIGHT * MIN_WINDOW_SIZE_MULTIPLIER);

    // Get gl context and set it to the current context for this window.
    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {FATAL_ERROR("Failed to create GL context."); }

    // Don't know if this is needed.
    glewInit();

    // Disable v-sync. The gameboy lcd is 60Hz and that will be synced manually.
    SDL_GL_SetSwapInterval(0);

    // Workaround for AMD. Must not be removed.
    if (!glBindFragDataLocation) {
        glBindFragDataLocation = glBindFragDataLocationEXT;
    }
}

void Application::terminate() {
    guiView.terminate();

    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Application::stepEmulation() {
    if (settings.emulationSpeedMultiplier == 1) {
        gameBoyStep();
    } else if (settings.emulationSpeedMultiplier < 1) {
        stepSlowly();
    } else {
        stepFast();
    }
}

void Application::stepFast() {
    for (int i = 0; i < settings.emulationSpeedMultiplier; i++) {
        gameBoyStep();
    }
}

void Application::stepSlowly() {
    if (framesUntilStep <= 0) {
        gameBoyStep();
        framesUntilStep = 1 / settings.emulationSpeedMultiplier;
    }
    framesUntilStep--;
}

void Application::gameBoyStep() {
    if (gameBoy.isReadyToDraw()) {
        //Actually discards frame until settings->playSpeed number of frames have been produced.
        gameBoy.confirmDraw();
    }
    while (!gameBoy.isReadyToDraw()) {
        gameBoy.step(&audio);
    }

    auto playSound = gameBoy.isReadyToPlaySound();
    if (playSound) {
        audio.stepSound(playSound, gameBoy.getAPUState());
        gameBoy.confirmPlay();
    }
}


void Application::correctWindowSize() {
    int newWidth, newHeight;
    SDL_SetWindowFullscreen(window, settings.fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
    SDL_GetWindowSize(window, &newWidth, &newHeight);

    if (windowWidth != newWidth || windowHeight != newHeight) {
        SDL_SetWindowSize(window, newWidth, newHeight);
        SDL_GetWindowSize(window, &windowWidth, &windowHeight);

        if (!settings.fullscreen) {
            settings.windowedWidth = windowWidth;
            settings.windowedHeight = windowHeight;
        }

        correctViewport();
    }
}

void Application::correctViewport() {
    if (settings.keepAspectRatio) {
        int viewportWidth, viewportHeight;
        int viewportX, viewportY;
        float screenMultiplier;

        screenMultiplier = std::min((float) windowWidth / LCD_WIDTH,
                                    (float) windowHeight / LCD_HEIGHT);

        viewportWidth = LCD_WIDTH * screenMultiplier;
        viewportHeight = LCD_HEIGHT * screenMultiplier;
        renderView.setViewportDim(viewportWidth, viewportHeight);

        viewportX = windowWidth * 0.5f - viewportWidth * 0.5f;
        viewportY = windowHeight * 0.5f - viewportHeight * 0.5f;
        renderView.setViewportPos(viewportX, viewportY);
    } else {
        renderView.setViewportDim(windowWidth, windowHeight);
        renderView.setViewportPos(0, 0);
    }
}


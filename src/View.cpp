//
// Created by isak_ on 2021-04-06.
//

#include "IO/View.h"

#include <utility>

View::View(std::shared_ptr<AppSettings> sharedPtr) {
    this->settings = std::move(sharedPtr);
}

void View::render(bool renderEmu,uint8_t textureData[]) {
    updateSDLWindowSize();
    renderView.clear();
    if(renderEmu){
        renderEmulation(textureData);
    }
  //  SDL_GL_SwapWindow(window);
}
void View::renderEmulation(uint8_t textureData[]) {
    renderView.setScreenTexture(textureData);
    renderView.render();
}

void View::renderGui() {
    gui.handleGui(this->window);
}

/**
 * Makes sure the window dimensions updates to match changes in RenderView dimensions.
 */
void View::updateSDLWindowSize() {
    int width;
    int height;
    SDL_GetWindowSize(window, &width, &height);
    if (width != renderView.getWidth() || height != renderView.getHeight()) {
        SDL_SetWindowSize(window, renderView.getWidth(), renderView.getHeight());
    }
}

void View::init(std::function<void(std::string)> &&loadRomCallback,int screenMultiplier) {
    initSDL();
    renderView.initGL();
    renderView.setScreenMultiplier(screenMultiplier);
    gui.init(window, &glContext, "#version 130", this->settings); // GLSL version
    gui.setLoadRomCallback(std::move(loadRomCallback));

}

/*
 * This function initializes SDL, creates a window and gets a OpenGL context. If an error occurs in this function
 * the program should exit.
 */
void View::initSDL() {
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
                              LCD_WIDTH,
                              LCD_HEIGHT,
                              SDL_WINDOW_OPENGL);
    if (!window) {FATAL_ERROR("Failed to create SDL window."); }

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

void View::terminateView() {
    gui.terminate();
    terminateSDL();
}

/**
 * Cleans up after SDL.
 */
void View::terminateSDL() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}


void View::swapbuffers() {
    SDL_GL_SwapWindow(window);
}

void View::toggleGui() {
    gui.toggleGui();
}

void View::handleGuiInput(SDL_Event event) {
    gui.handleInput(event);
}



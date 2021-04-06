//
// Created by isak_ on 2021-04-06.
//

#include "View.h"


void View::RenderGUI(){

}

void View::RenderEmulation(){
    updateSDLWindowSize();
    renderView.clear();

    // Handle Events
    handleSDLEvents();
    if (state == State::EMULATION && gameBoy.isOn()) {

        renderEmulation();
    }

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


/**
 * Handles SDL Events including keyboard input.
 */
void View::handleSDLEvents() {
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
                if (key == settings->keyBinds.turboMode.keyval) {
                    savedEmulationSpeed = settings->emulationSpeedMultiplier;
                    settings->emulationSpeedMultiplier = MAX_EMULATION_SPEED_FLOAT;
                }
                if (state == State::EMULATION) {
                    handleEmulatorInputPress(key);
                }
                break;

            case SDL_KEYUP:
                if (key == settings->keyBinds.turboMode.keyval) {
                    settings->emulationSpeedMultiplier = savedEmulationSpeed;
                }
                if (state == State::EMULATION) {
                    handleEmulatorInputRelease(key);
                }
                break;
        }
    }
}

void View::handleEmulatorInputPress(SDL_Keycode key) {
    // Left and right can not be pressed simultaneously, the same goes for up and down!
    if (key == settings->keyBinds.left.keyval) {
        gameBoy.joypad_input(JOYPAD_RIGHT, JOYPAD_RELEASE);
        gameBoy.joypad_input(JOYPAD_LEFT, JOYPAD_PRESS);
    } else if (key == settings->keyBinds.right.keyval) {
        gameBoy.joypad_input(JOYPAD_LEFT, JOYPAD_RELEASE);
        gameBoy.joypad_input(JOYPAD_RIGHT, JOYPAD_PRESS);
    } else if (key == settings->keyBinds.up.keyval) {
        gameBoy.joypad_input(JOYPAD_DOWN, JOYPAD_RELEASE);
        gameBoy.joypad_input(JOYPAD_UP, JOYPAD_PRESS);
    } else if (key == settings->keyBinds.down.keyval) {
        gameBoy.joypad_input(JOYPAD_UP, JOYPAD_RELEASE);
        gameBoy.joypad_input(JOYPAD_DOWN, JOYPAD_PRESS);
    } else if (key == settings->keyBinds.a.keyval) {
        gameBoy.joypad_input(JOYPAD_A, JOYPAD_PRESS);
    } else if (key == settings->keyBinds.b.keyval) {
        gameBoy.joypad_input(JOYPAD_B, JOYPAD_PRESS);
    } else if (key == settings->keyBinds.start.keyval) {
        gameBoy.joypad_input(JOYPAD_START, JOYPAD_PRESS);
    } else if (key == settings->keyBinds.select.keyval) {
        gameBoy.joypad_input(JOYPAD_SELECT, JOYPAD_PRESS);
    }
}

void View::handleEmulatorInputRelease(SDL_Keycode key) {
    if (key == settings->keyBinds.left.keyval) {
        gameBoy.joypad_input(JOYPAD_LEFT, JOYPAD_RELEASE);
    } else if (key == settings->keyBinds.right.keyval) {
        gameBoy.joypad_input(JOYPAD_RIGHT, JOYPAD_RELEASE);
    } else if (key == settings->keyBinds.up.keyval) {
        gameBoy.joypad_input(JOYPAD_UP, JOYPAD_RELEASE);
    } else if (key == settings->keyBinds.down.keyval) {
        gameBoy.joypad_input(JOYPAD_DOWN, JOYPAD_RELEASE);
    } else if (key == settings->keyBinds.a.keyval) {
        gameBoy.joypad_input(JOYPAD_A, JOYPAD_RELEASE);
    } else if (key == settings->keyBinds.b.keyval) {
        gameBoy.joypad_input(JOYPAD_B, JOYPAD_RELEASE);
    } else if (key == settings->keyBinds.start.keyval) {
        gameBoy.joypad_input(JOYPAD_START, JOYPAD_RELEASE);
    } else if (key == settings->keyBinds.select.keyval) {
        gameBoy.joypad_input(JOYPAD_SELECT, JOYPAD_RELEASE);
    }
}
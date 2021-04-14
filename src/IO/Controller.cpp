//
// Created by isak_ on 2021-04-06.
//

#include "Controller.h"
Controller::Controller(AppSettings& settings, GuiView& guiView, GameBoy& gameBoy):
    settings{settings}, guiView{guiView}, gameBoy{gameBoy}
{
    savedEmulationSpeed = settings.emulationSpeedMultiplier;
}

/**
 * Handles SDL Events including keyboard input.
 */
State Controller::handleSDLEvents(State state) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        SDL_Keycode key = event.key.keysym.sym;

        if (state == State::MENU) {
            guiView.handleInput(event);
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
                    guiView.toggleGui();
                    state = (state == State::EMULATION) ? State::MENU : State::EMULATION;
                }
                if (key == settings.keyBinds.turboMode.keyval) {
                    savedEmulationSpeed = settings.emulationSpeedMultiplier;
                    settings.emulationSpeedMultiplier = MAX_EMULATION_SPEED_FLOAT;
                }
                if (state == State::EMULATION) {
                    handleEmulatorInputPress(key);
                }
                break;

            case SDL_KEYUP:
                if (key == settings.keyBinds.turboMode.keyval) {
                    settings.emulationSpeedMultiplier = savedEmulationSpeed;
                }
                if (state == State::EMULATION) {
                    handleEmulatorInputRelease(key);
                }
                break;
        }
    }
    return state;
}

void Controller::handleEmulatorInputPress(SDL_Keycode key) {
    // Left and right can not be pressed simultaneously, the same goes for up and down!
    if (key == settings.keyBinds.left.keyval) {
        gameBoy.joypad_input(JOYPAD_RIGHT, JOYPAD_RELEASE);
        gameBoy.joypad_input(JOYPAD_LEFT, JOYPAD_PRESS);
    } else if (key == settings.keyBinds.right.keyval) {
        gameBoy.joypad_input(JOYPAD_LEFT, JOYPAD_RELEASE);
        gameBoy.joypad_input(JOYPAD_RIGHT, JOYPAD_PRESS);
    } else if (key == settings.keyBinds.up.keyval) {
        gameBoy.joypad_input(JOYPAD_DOWN, JOYPAD_RELEASE);
        gameBoy.joypad_input(JOYPAD_UP, JOYPAD_PRESS);
    } else if (key == settings.keyBinds.down.keyval) {
        gameBoy.joypad_input(JOYPAD_UP, JOYPAD_RELEASE);
        gameBoy.joypad_input(JOYPAD_DOWN, JOYPAD_PRESS);
    } else if (key == settings.keyBinds.a.keyval) {
        gameBoy.joypad_input(JOYPAD_A, JOYPAD_PRESS);
    } else if (key == settings.keyBinds.b.keyval) {
        gameBoy.joypad_input(JOYPAD_B, JOYPAD_PRESS);
    } else if (key == settings.keyBinds.start.keyval) {
        gameBoy.joypad_input(JOYPAD_START, JOYPAD_PRESS);
    } else if (key == settings.keyBinds.select.keyval) {
        gameBoy.joypad_input(JOYPAD_SELECT, JOYPAD_PRESS);
    }
}


void Controller::handleEmulatorInputRelease(SDL_Keycode key) {
    if (key == settings.keyBinds.left.keyval) {
        gameBoy.joypad_input(JOYPAD_LEFT, JOYPAD_RELEASE);
    } else if (key == settings.keyBinds.right.keyval) {
        gameBoy.joypad_input(JOYPAD_RIGHT, JOYPAD_RELEASE);
    } else if (key == settings.keyBinds.up.keyval) {
        gameBoy.joypad_input(JOYPAD_UP, JOYPAD_RELEASE);
    } else if (key == settings.keyBinds.down.keyval) {
        gameBoy.joypad_input(JOYPAD_DOWN, JOYPAD_RELEASE);
    } else if (key == settings.keyBinds.a.keyval) {
        gameBoy.joypad_input(JOYPAD_A, JOYPAD_RELEASE);
    } else if (key == settings.keyBinds.b.keyval) {
        gameBoy.joypad_input(JOYPAD_B, JOYPAD_RELEASE);
    } else if (key == settings.keyBinds.start.keyval) {
        gameBoy.joypad_input(JOYPAD_START, JOYPAD_RELEASE);
    } else if (key == settings.keyBinds.select.keyval) {
        gameBoy.joypad_input(JOYPAD_SELECT, JOYPAD_RELEASE);
    }
}




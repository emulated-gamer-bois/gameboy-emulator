#include "Controller.h"

Controller::Controller(AppSettings& settings, GuiView& guiView, GameBoy& gameBoy):
    settings{settings}, guiView{guiView}, gameBoy{gameBoy}
{
    savedEmulationSpeed = settings.emulationSpeedMultiplier;
}

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
                if (key == settings.keyBinds.turboMode.keyVal) {
                    savedEmulationSpeed = settings.emulationSpeedMultiplier;
                    settings.emulationSpeedMultiplier = MAX_EMULATION_SPEED_FLOAT;
                }
                if (state == State::EMULATION) {
                    handleEmulatorInputPress(key);
                }
                break;

            case SDL_KEYUP:
                if (key == settings.keyBinds.turboMode.keyVal) {
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
    if (key == settings.keyBinds.left.keyVal) {
        gameBoy.joypadInput(JOYPAD_RIGHT, JOYPAD_RELEASE);
        gameBoy.joypadInput(JOYPAD_LEFT, JOYPAD_PRESS);
    } else if (key == settings.keyBinds.right.keyVal) {
        gameBoy.joypadInput(JOYPAD_LEFT, JOYPAD_RELEASE);
        gameBoy.joypadInput(JOYPAD_RIGHT, JOYPAD_PRESS);
    } else if (key == settings.keyBinds.up.keyVal) {
        gameBoy.joypadInput(JOYPAD_DOWN, JOYPAD_RELEASE);
        gameBoy.joypadInput(JOYPAD_UP, JOYPAD_PRESS);
    } else if (key == settings.keyBinds.down.keyVal) {
        gameBoy.joypadInput(JOYPAD_UP, JOYPAD_RELEASE);
        gameBoy.joypadInput(JOYPAD_DOWN, JOYPAD_PRESS);
    } else if (key == settings.keyBinds.a.keyVal) {
        gameBoy.joypadInput(JOYPAD_A, JOYPAD_PRESS);
    } else if (key == settings.keyBinds.b.keyVal) {
        gameBoy.joypadInput(JOYPAD_B, JOYPAD_PRESS);
    } else if (key == settings.keyBinds.start.keyVal) {
        gameBoy.joypadInput(JOYPAD_START, JOYPAD_PRESS);
    } else if (key == settings.keyBinds.select.keyVal) {
        gameBoy.joypadInput(JOYPAD_SELECT, JOYPAD_PRESS);
    }
}

void Controller::handleEmulatorInputRelease(SDL_Keycode key) {
    if (key == settings.keyBinds.left.keyVal) {
        gameBoy.joypadInput(JOYPAD_LEFT, JOYPAD_RELEASE);
    } else if (key == settings.keyBinds.right.keyVal) {
        gameBoy.joypadInput(JOYPAD_RIGHT, JOYPAD_RELEASE);
    } else if (key == settings.keyBinds.up.keyVal) {
        gameBoy.joypadInput(JOYPAD_UP, JOYPAD_RELEASE);
    } else if (key == settings.keyBinds.down.keyVal) {
        gameBoy.joypadInput(JOYPAD_DOWN, JOYPAD_RELEASE);
    } else if (key == settings.keyBinds.a.keyVal) {
        gameBoy.joypadInput(JOYPAD_A, JOYPAD_RELEASE);
    } else if (key == settings.keyBinds.b.keyVal) {
        gameBoy.joypadInput(JOYPAD_B, JOYPAD_RELEASE);
    } else if (key == settings.keyBinds.start.keyVal) {
        gameBoy.joypadInput(JOYPAD_START, JOYPAD_RELEASE);
    } else if (key == settings.keyBinds.select.keyVal) {
        gameBoy.joypadInput(JOYPAD_SELECT, JOYPAD_RELEASE);
    }
}
// Uncomment if opengl won't work
/*
#ifdef _WIN32
extern "C" _declspec(dllexport) unsigned int NvOptimusEnablement = 0x00000001;
#endif
*/

#include "Application.h"
#include "../helpers/AppTimer.h"

/**
 * Constructor
 */
Application::Application() {
    state = State::MENU;
    initSettings();
}

/**
 */
void Application::start() {
    init();
    float frameTime = 1000.f / LCD_REFRESH_RATE;
    AppTimer timer;
    state=State::MENU;
    while (state != State::TERMINATION) {
        // Create time stamp.
        timer.tick();

        // Step through emulation until playspeed number of frames are produced, then display the last one.
        if (state == State::EMULATION && gameBoy->isOn()) {
            stepEmulation();
            if(gameBoy->isReadyToDraw()) {
                gameBoy->confirmDraw();
            }
        }
        view->render(state == State::EMULATION && gameBoy->isOn(),gameBoy->getScreenTexture().get());
        controller.handleSDLEvents();

        // Render menu
        if (state == State::MENU) {
            audio.stopSource(0);
            audio.stopSource(1);
            audio.stopSource(2);
            view->renderGui();
        }
        view->swapbuffers();
        // Swap buffers
       // SDL_GL_SwapWindow(window);

        // Time application to 60Hz
        float msSinceTick = timer.msSinceTick();
        if (msSinceTick < frameTime) {
            int msToSleep = frameTime - msSinceTick;
            std::this_thread::sleep_for(std::chrono::milliseconds(msToSleep));
        }
    }
    if (this->gameBoy->save()) {
        std::cout << "Saved successfully" << std::endl;
    } else {
        std::cout << "Save failed" << std::endl;
    }
    terminate();
}

void Application::init() {
    view->init(    ([this](std::string &&romPath) -> void {
         gameBoy->load_rom("../roms/gb/boot_lameboy_big.gb", romPath);
        state = State::EMULATION;
        view->toggleGui();
    }),settings->screenMultiplier);
}

void Application::terminate() {
    view->terminateView();
}



/**
 * Initialize settings to some default values. This should be called if settings couldn't be loaded from file.
 */
void Application::initSettings() {
    settings->keyBinds.init_keybinds();
    settings->screenMultiplier = 4;
    settings->romPath = "..";
    settings->emulationSpeedMultiplier = 1;
}

void Application::updateSound(uint8_t ready) {
    //Play audio
    //this->audio.playBuffers(this->gameBoy->getAudioBuffers());

    auto state = this->gameBoy->getAPUState();

    //1st square
    if (ready & 1) {
        this->audio.stopSource(0);
        if (state->enable_square_a) {
            this->audio.playGBSquare(0, state->duty_square_a, state->frequency_square_a, state->volume_square_a);
        }
    }

    //2nd square
    if (ready & 2) {
        this->audio.stopSource(1);
        if (state->enable_square_b) {
            this->audio.playGBSquare(1, state->duty_square_b, state->frequency_square_b, state->volume_square_b);
        }
    }

    //Wave
    if (ready & 4) {
        this->audio.stopSource(2);
        if (state->enable_wave) {
            this->audio.playGBWave(2, state->waveform_wave, state->frequency_wave, state->volume_wave);
        }
    }

    gameBoy->confirmPlay();
    delete state;
}

void Application::stepFast() {
    for (int i = 0; i < settings->emulationSpeedMultiplier; i++) {
        gameBoyStep();
    }
}

void Application::stepSlowly() {
    if (framesUntilStep == 0) {
        gameBoyStep();
        framesUntilStep = 1 / settings->emulationSpeedMultiplier;
    }
    framesUntilStep--;
}

void Application::gameBoyStep() {
    if (gameBoy->isReadyToDraw()) {
        //Actually discards frame until settings->playSpeed number of frames have been produced.
        gameBoy->confirmDraw();
    }
    while (!gameBoy->isReadyToDraw()) {
        gameBoy->step(&audio);
    }
    uint8_t playSound = this->gameBoy->isReadyToPlaySound();
    if (playSound) {
        updateSound(playSound);
    }
}

void Application::stepEmulation() {
    if (settings->emulationSpeedMultiplier == 1) {
        gameBoyStep();
    } else if (settings->emulationSpeedMultiplier < 1) {
        stepSlowly();
    } else {
        stepFast();
    }
}


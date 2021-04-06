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
    state = State::MENU;
    while (state != State::TERMINATION) {
        // Create time stamp.
        timer.tick();

        // Step through emulation until playspeed number of frames are produced, then display the last one.
        if (state == State::EMULATION && gameBoy->isOn()) {
            stepEmulation();
            if (gameBoy->isReadyToDraw()) {
                gameBoy->confirmDraw();
            }
        }
        view->render(state == State::EMULATION && gameBoy->isOn(), gameBoy->getScreenTexture().get());
        this->state = controller.handleSDLEvents(state);

        // Render menu
        if (state == State::MENU) {
            audio.stopSound();
            view->renderGui();
        }
        view->swapbuffers();

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
    view->init(([this](std::string &&romPath) -> void {
        gameBoy->load_rom("../roms/gb/boot_lameboy_big.gb", romPath);
        state = State::EMULATION;
        view->toggleGui();
    }), settings->screenMultiplier);
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
        audio.stepSound(playSound, this->gameBoy->getAPUState());
        gameBoy->confirmPlay();
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


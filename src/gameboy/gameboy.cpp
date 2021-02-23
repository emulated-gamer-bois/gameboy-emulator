//
// Created by algot on 2021-02-23.
//

#include "gameboy.h"

GameBoy::GameBoy() {
    this->mmu = std::make_shared<MMU>();
    this->cpu = std::make_unique<CPU>(0x0000, 0xFFFE, mmu);
}

void GameBoy::step() {
    uint8_t cycles;
    // cycles = this->cpu->execute_instruction();
    //                           TODO: Wait for execute_instruction to return clocks
    // this->ppu.update(cycles); TODO: Wait for ppu implementation
    // mmu.timerUpdate(cycles);  TODO: Wait for mmu timer implementation
}

uint8_t *GameBoy::getScreen() {
    return nullptr;
}

void GameBoy::joypad_input(uint8_t key, uint8_t action) {
    switch (action) {
        case JOYPAD_PRESS:
            this->mmu->joypad_press(key);
            break;
        case JOYPAD_RELEASE:
            this->mmu->joypad_release(key);
            break;

        default:
            std::cout << "Invalid parameter `action` to GameBoy::joypad_input: " << action << std::endl;
            break;
  }
}

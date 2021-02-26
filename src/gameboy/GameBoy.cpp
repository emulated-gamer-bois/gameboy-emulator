//
// Created by algot on 2021-02-23.
//

#include "GameBoy.h"

GameBoy::GameBoy() {
    this->mmu = std::make_shared<MMU>();
    this->cpu = std::make_unique<CPU>(0x0000, 0xFFFE, mmu);
    this->ppu = std::make_unique<PPU>(mmu);
}

void GameBoy::step() {
    // TODO: Check for interrupts
    int cycles;
    cycles = this->cpu->execute_instruction();
    ppu->update(cycles);
    mmu->timer_update(cycles);
}

uint8_t *GameBoy::getScreen() {
    uint8_t* screenBuffer = nullptr;
    if (ppu->getMode() == PPU::VBLANK) {
        screenBuffer = ppu->getFrameBuffer();
    }

    return screenBuffer;
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

void GameBoy::load_game_rom(std::string filepath) {
    this->mmu->load_game_rom(filepath);
}

void GameBoy::load_boot_rom(std::string filepath) {
    this->mmu->load_boot_rom(filepath);
}

void GameBoy::cpu_dump() {
    this->cpu->cpu_dump();
}

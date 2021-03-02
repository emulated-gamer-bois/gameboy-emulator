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
    int cycles = this->cpu->update();
    this->ppu->update(cycles);
    this->mmu->timer_update(cycles);
}

uint8_t *GameBoy::getScreen() {
    return this->ppu->getFrameBuffer()->data();
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

bool GameBoy::isReadyToDraw() const {
    return this->ppu->isReadyToDraw();
}

void GameBoy::confirmDraw() {
    this->ppu->confirmDraw();
}

void GameBoy::handleInterrupts() {
    uint8_t flags = mmu->read(INTERRUPT_FLAG);
    uint8_t mask = mmu->read(INTERRUPT_ENABLE);
    uint8_t maskedFlags = flags & mask;
    if (maskedFlags & (1 << 0)) { //V-Blank interrupt

    } else if (maskedFlags & (1 << 1)) { //STAT interrupt

    }
    //TODO remaining interrupts
}

//
// Created by algot on 2021-02-23.
//

#include "GameBoy.h"

GameBoy::GameBoy() {
    this->mmu = std::make_shared<MMU>();
    this->cpu = std::make_unique<CPU>(0x0000, 0xFFFE, mmu);
    this->ppu = std::make_unique<PPU>(mmu);
    this->on = false;
}

void GameBoy::step() {
    if (!this->on) {
        return;
    }

    int cycles = this->cpu->update();
    this->ppu->update(cycles);
    this->mmu->timer_update(cycles);
// =======
//     if (!this->cpu->getStop()) {
//         // TODO: Check for interrupts
//         int cycles = 0;
//         if (!this->cpu->getHalt()) {
//             cycles = this->cpu->execute_instruction();
//
//         }else if(mmu->read(0xffff) & mmu->read(0xFF0F) & 0x1f){
//
//         }
//         this->ppu->update(cycles);
//         this->mmu->timer_update(cycles);
//     } else if (mmu->read(0xFFFF)) {
//         cpu->return_from_stop();
//     }
//
//
// >>>>>>> cpu-bug-fixes
}

std::unique_ptr<uint8_t[]> GameBoy::getScreenTexture() {
    auto ppuFrameBuffer = this->ppu->getFrameBuffer();
    auto texture = std::make_unique<uint8_t[]>(ppuFrameBuffer->size());

    for (int i = 0; i < ppuFrameBuffer->size(); i++) {
        texture[i] = 0xFF - (ppuFrameBuffer->at(i) * 0x55);
    }

    return texture;
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

void GameBoy::load_rom(std::string bootFilepath, std::string romFilepath) {
    this->cpu->reset();
    this->ppu->reset();
    this->mmu->reset();
    if (!this->mmu->load_boot_rom(bootFilepath)) {
        this->cpu->skipBootRom();
    }
    on = this->mmu->load_game_rom(romFilepath);
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

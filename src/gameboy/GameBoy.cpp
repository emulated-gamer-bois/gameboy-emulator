//
// Created by algot on 2021-02-23.
//

#include "GameBoy.h"

GameBoy::GameBoy() {
    this->mmu = std::make_shared<MMU>();

    this->joypad = std::make_shared<Joypad>(this->mmu);
    this->timer = std::make_shared<Timer>(this->mmu);
    this->cartridge = std::make_shared<Cartridge>();

    this->cpu = std::make_unique<CPU>(0x0000, 0xFFFE, mmu);
    this->ppu = std::make_shared<PPU>(mmu);
    this->apu = std::make_shared<APU>(mmu);
    this->mmu->link_devices(this->ppu, this->apu, this->joypad, this->timer, this->cartridge);
    this->on = false;
}

void GameBoy::step() {
    if (!this->on) {
        return;
    }

    int cycles = this->cpu->update();
    this->ppu->update(cycles);
    this->apu->update(cycles);
    this->timer->update(cycles);
    this->cartridge->update(cycles);
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
            this->joypad->press(key);
            break;
        case JOYPAD_RELEASE:
            this->joypad->release(key);
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
    this->timer->reset();
    this->joypad->reset();
    if (!this->mmu->load_boot_rom(bootFilepath)) {
        this->cpu->skipBootRom();
    }
    this->on = this->cartridge->load_rom(romFilepath);
}

void GameBoy::load_game_rom(std::string filepath) {
    this->cartridge->load_rom(filepath);
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
bool GameBoy::isReadyToPlaySound() {
    return this->apu->isReadyToPlaySound();
}
void GameBoy::confirmPlay() {
    this->apu->confirmPlay();
}
std::shared_ptr<APUState> GameBoy::getState() {
    return this->apu->getState();
}

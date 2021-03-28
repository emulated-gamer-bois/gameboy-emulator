//
// Created by algot on 2021-02-23.
//

#include "GameBoy.h"

GameBoy::GameBoy() {
    mmu = std::make_shared<MMU>();

    joypad = std::make_shared<Joypad>(mmu);
    timer = std::make_shared<Timer>(mmu);
    cartridge = std::make_shared<Cartridge>();

    cpu = std::make_unique<CPU>(0x0000, 0xFFFE, mmu);
    ppu = std::make_shared<PPU>(mmu);
    mmu->link_devices(ppu, joypad, timer, cartridge);
    on = false;
}

void GameBoy::step() {
    if (!on) {
        return;
    }

    int cycles = cpu->update();
    ppu->update(cycles);
    timer->update(cycles);
    cartridge->update(cycles);
}

std::unique_ptr<uint8_t[]> GameBoy::getScreenTexture() {
    auto ppuFrameBuffer = ppu->getFrameBuffer();
    auto texture = std::make_unique<uint8_t[]>(ppuFrameBuffer->size());

    for (int i = 0; i < ppuFrameBuffer->size(); i++) {
        texture[i] = 0xFF - (ppuFrameBuffer->at(i) * 0x55);
    }
    return texture;
}

void GameBoy::joypad_input(uint8_t key, uint8_t action) {
    switch (action) {
        case JOYPAD_PRESS:
            joypad->press(key);
            break;
        case JOYPAD_RELEASE:
            joypad->release(key);
            break;
        default:
            std::cerr << "Invalid parameter `action` to GameBoy::joypad_input: " << action << std::endl;
            break;
    }
}

void GameBoy::load_rom(std::string bootFilepath, std::string romFilepath) {
    cpu->reset();
    ppu->reset();
    mmu->reset();
    timer->reset();
    joypad->reset();
    if (!mmu->load_boot_rom(bootFilepath)) {
        cpu->skipBootRom();
    }
    on = cartridge->load_rom(romFilepath);
}

void GameBoy::load_game_rom(std::string filepath) {
    cartridge->load_rom(filepath);
}

void GameBoy::load_boot_rom(std::string filepath) {
    mmu->load_boot_rom(filepath);
}

void GameBoy::cpu_dump() {
    cpu->cpu_dump();
}

bool GameBoy::isOn() const {
    return on;
}

bool GameBoy::isReadyToDraw() const {
    return ppu->isReadyToDraw();
}

void GameBoy::confirmDraw() {
    ppu->confirmDraw();
}

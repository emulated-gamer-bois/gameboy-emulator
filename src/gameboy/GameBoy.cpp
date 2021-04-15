//
// Created by algot on 2021-02-23.
//

#include "GameBoy.h"

GameBoy::GameBoy()
{
    mmu = std::make_shared<MMU>();

    joypad = std::make_shared<Joypad>(mmu);
    timer = std::make_shared<Timer>(mmu);
    cartridge = std::make_shared<Cartridge>();

    cpu = std::make_unique<CPU>(0x0000, 0xFFFE, mmu);
    ppu = std::make_shared<PPU>(mmu);
    apu = std::make_shared<APU>();
    mmu->linkDevices(this->ppu, this->apu, this->joypad, this->timer, this->cartridge);
    on = false;
}

void GameBoy::step(IVolumeController *vc)
{
    if (!this->on)
    {
        return;
    }
    if(cpu->getStop()){
        if(mmu->read(JOYPAD) & mmu->read(INTERRUPT_ENABLE)){
            cpu->returnFromStop();
        }
        return;
    }


    int cycles = this->cpu->update();
    ppu->update(cycles);
    apu->update(cycles, vc);
    timer->update(cycles);
    cartridge->update(cycles);
}

std::unique_ptr<uint8_t[]> GameBoy::getScreenTexture()
{
    auto ppuFrameBuffer = ppu->getFrameBuffer();
    auto texture = std::make_unique<uint8_t[]>(ppuFrameBuffer->size());

    for (int i = 0; i < ppuFrameBuffer->size(); i++)
    {
        texture[i] = 0xFF - (ppuFrameBuffer->at(i) * 0x55);
    }
    return texture;
}

void GameBoy::joypad_input(uint8_t key, uint8_t action)
{
    switch (action)
    {
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

void GameBoy::load_rom(std::string bootFilepath, std::string romFilepath)
{
    cpu->reset();
    ppu->reset();
    apu->reset();
    mmu->reset();
    timer->reset();
    joypad->reset();
    if (!mmu->loadBootRom(bootFilepath))
    {
        cpu->skipBootRom();
    }
    on = cartridge->loadRom(romFilepath, true);
}

void GameBoy::load_game_rom(std::string filepath)
{
    cartridge->loadRom(filepath);
}

void GameBoy::load_boot_rom(std::string filepath)
{
    mmu->loadBootRom(filepath);
}

void GameBoy::cpu_dump()
{
    cpu->cpuDump();
}

bool GameBoy::isOn() const
{
    return on;
}

bool GameBoy::isReadyToDraw() const
{
    return ppu->isReadyToDraw();
}

void GameBoy::confirmDraw()
{
    ppu->confirmDraw();
}

uint8_t GameBoy::isReadyToPlaySound()
{
    return this->apu->isReadyToPlaySound();
}
void GameBoy::confirmPlay()
{
    this->apu->confirmPlay();
}
APUState *GameBoy::getAPUState()
{
    return this->apu->getAPUState();
}

bool GameBoy::save() {
    // Save RAM to separate file
    if (!this->cartridge->saveRam()) {
        return false;
    }
    return true;
}

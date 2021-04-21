//
// Created by algot on 2021-02-23.
//

#pragma once

#include <memory>

#include "CPU/CPU.h"
#include "MMU/MMU.h"
#include "PPU/PPU.h"
#include "Joypad.h"
#include "MMU/Timer.h"
#include "MMU/Cartridge.h"

#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test

#define JOYPAD_RELEASE 0
#define JOYPAD_PRESS   1

class GameBoy {
public:
    GameBoy();
    void step(IVolumeController* vc);
    std::unique_ptr<uint8_t[]> getScreenTexture();
    void joypadInput(uint8_t key, uint8_t action);
    void loadRom(std::string bootFilepath, std::string romFilepath);
    void loadGameRom(std::string filepath);
    void loadBootRom(std::string filepath);
    bool isReadyToDraw() const;
    void confirmDraw();
    void cpuDump();
    bool isOn() const;

    bool save();

    uint8_t isReadyToPlaySound();
    void confirmPlay();
    APUState* getAPUState();

private:
    bool on;

    std::shared_ptr<MMU> mmu;
    std::unique_ptr<CPU> cpu;
    std::shared_ptr<PPU> ppu;
    std::shared_ptr<APU> apu;

    std::shared_ptr<Joypad> joypad;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<Cartridge> cartridge;

    FRIEND_TEST(PPU, g_tile_rom);
};

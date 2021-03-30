//
// Created by algot on 2021-02-23.
//

#pragma once

#include <memory>

#include "CPU.h"
#include "MMU.h"
#include "PPU/PPU.h"
#include "Joypad.h"
#include "Timer.h"
#include "Cartridge.h"

#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test

#define JOYPAD_RELEASE 0
#define JOYPAD_PRESS   1

class GameBoy {
public:
    GameBoy();
    void step();
    std::unique_ptr<uint8_t[]> getScreenTexture();
    void joypad_input(uint8_t key, uint8_t action);
    void load_rom(std::string bootFilepath, std::string romFilepath);
    void load_game_rom(std::string filepath);
    void load_boot_rom(std::string filepath);
    bool isReadyToDraw() const;
    void confirmDraw();
    void cpu_dump();
    bool isOn() const;

    bool save();

private:
    bool on;

    std::shared_ptr<MMU> mmu;
    std::unique_ptr<CPU> cpu;
    std::shared_ptr<PPU> ppu;

    std::shared_ptr<Joypad> joypad;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<Cartridge> cartridge;

    FRIEND_TEST(PPU, g_tile_rom);
};

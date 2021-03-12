//
// Created by algot on 2021-02-15.
//

#pragma once

#include "Cartridge.h"
#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

// Forward declaration
class PPU;
class Timer;
class Joypad;

#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test

// Addresses
#define ADDR_SPACE_START    0x0000
#define BOOT_ROM_START      0x0000
#define BOOT_ROM_END        0x00ff
#define GAME_ROM_START      0x0000
#define GAME_ROM_END        0x7fff
#define VRAM_START          0x8000
#define VRAM_END            0x9fff
#define xRAM_START          0xa000
#define xRAM_END            0xbfff
#define RAM_START           0xc000
#define RAM_END             0xdfff
#define OAM_START           0xfe00
#define OAM_END             0xfe9f
#define IO_START            0xff00
#define IO_END              0xff7f
#define HRAM_START          0xff80
#define HRAM_END            0xfffe
#define INTERRUPT_ENABLE    0xffff
#define ADDR_SPACE_END      0xffff
#define IO_DISABLE_BOOT_ROM 0xff50
#define IO_JOYPAD           0xff00
#define INTERRUPT_FLAG      0xff0f
#define DMA_TRANSFER        0xff46

class MMU {
public:
    MMU();
    void reset();
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);

    void raise_interrupt_flag(uint8_t bitmask);
    void clear_interrupt_flag(uint8_t bitmask);

    void link_devices(std::shared_ptr<PPU> ppu, std::shared_ptr<Joypad> joypad, std::shared_ptr<Timer> timer);

    bool load_game_rom(std::string filepath);
    bool load_boot_rom(std::string filepath);
    //void joypad_release(uint8_t button);
    //void joypad_press(uint8_t button);
    //void timer_update(uint16_t cycles);

private:
    void write_GAME_ROM_ONLY_IN_TESTS(uint16_t addr, uint8_t data);
    void write_BOOT_ROM_ONLY_IN_TESTS(uint16_t addr, uint8_t data);
    /*void write_io(uint16_t addr, uint8_t data);
    uint8_t read_io(uint16_t addr);*/
    void disable_boot_rom(uint8_t data);

    std::unique_ptr<Cartridge> cartridge;
    std::shared_ptr<Joypad> joypad;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<PPU> ppu;

    // Using array for memory with fixed size.
    std::array<uint8_t, 256> boot_rom;
    std::array<uint8_t, 8192> vram;
    std::array<uint8_t, 8192> ram;
    std::array<uint8_t, 160> oam;
    std::array<uint8_t, 128> io;
    std::array<uint8_t, 128> hram;

    // Fix for Cartridge type 0x01: MBC1 with no RAM or battery
    //uint8_t rom_bank_number;
    //uint8_t cartridgeType;

    bool booting;
    uint8_t interrupt_enable;
    uint8_t interrupt_flag;
    //uint8_t io_joypad_select;
    //uint8_t io_joypad;

    // Tests using private stuff
    FRIEND_TEST(MMU, read_write);
    FRIEND_TEST(MMU, disable_boot_rom);
    FRIEND_TEST(CPU, Execute_NOP_Instruction);
    FRIEND_TEST(CPU, Execute_LD_SP_D16_Instruction);
    FRIEND_TEST(CPU, FUNDAMENTAL_FUNCTIONS);
};

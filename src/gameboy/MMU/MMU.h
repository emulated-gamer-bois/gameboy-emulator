//
// Created by algot on 2021-02-15.
//

#pragma once

#include "Cartridge.h"
#include "../APU/APU.h"
#include <cstdint>
#include <array> // array
#include <string> // string

// Forward declaration
class PPU;
class APU;
class Timer;
class Joypad;

#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test

// Addresses
#define BOOT_ROM_START          0x0000
#define BOOT_ROM_END            0x00ff
#define GAME_ROM_START          0x0000
#define GAME_ROM_END            0x7fff
#define VRAM_START              0x8000
#define VRAM_END                0x9fff
#define xRAM_START              0xa000
#define xRAM_END                0xbfff
#define WRAM_START              0xc000
#define WRAM_END                0xdfff
#define ECHO_RAM_START          0xe000
#define ECHO_RAM_END            0xfdff
#define OAM_START               0xfe00
#define OAM_END                 0xfe9f
#define PROHIBITED_START        0xfea0
#define PROHIBITED_END          0xfeff
#define IO_START                0xff00
#define IO_END                  0xff7f
#define HRAM_START              0xff80
#define HRAM_END                0xfffe
#define INTERRUPT_ENABLE        0xffff

#define IO_JOYPAD               0xff00
#define IO_SERIAL_DATA_START    0xff01
#define IO_SERIAL_DATA_END      0xff02
#define IO_TIMER_START          0xff04
#define IO_TIMER_END            0xff07
#define INTERRUPT_FLAG          0xff0f
#define IO_SOUND_START          0xff10
#define IO_SOUND_END            0xff3f
#define IO_WAVEFORM_RAM_START   0xff30
#define IO_WAVEFORM_RAM_END     0xff3f
#define IO_LCD_START            0xff40
#define IO_LCD_END              0xff4b
#define IO_DISABLE_BOOT_ROM     0xff50

class MMU {
public:
    MMU();
    void reset();
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);

    void raise_interrupt_flag(uint8_t bitmask);
    void clear_interrupt_flag(uint8_t bitmask);

    void link_devices(std::shared_ptr<PPU> ppu, std::shared_ptr<APU> apu, std::shared_ptr<Joypad> joypad, std::shared_ptr<Timer> timer, std::shared_ptr<Cartridge> cartridge);

    bool load_boot_rom(const std::string& filepath);

private:
    void write_GAME_ROM_ONLY_IN_TESTS(uint16_t addr, uint8_t data);
    void write_BOOT_ROM_ONLY_IN_TESTS(uint16_t addr, uint8_t data);
    void disable_boot_rom(uint8_t data);

    // Devices
    std::shared_ptr<Cartridge> cartridge;
    std::shared_ptr<Joypad> joypad;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<PPU> ppu;
    std::shared_ptr<APU> apu;

    // Using array for memory with fixed size.
    std::array<uint8_t, 256> boot_rom;
    std::array<uint8_t, 8192> vram;
    std::array<uint8_t, 8192> ram;
    std::array<uint8_t, 160> oam;
    std::array<uint8_t, 128> hram;

    bool booting;
    uint8_t interrupt_enable;
    uint8_t interrupt_flag;

    // Tests using private stuff
    FRIEND_TEST(MMU, read_write);
    FRIEND_TEST(MMU, disable_boot_rom);
    FRIEND_TEST(CPU, Execute_NOP_Instruction);
    FRIEND_TEST(CPU, Execute_LD_SP_D16_Instruction);
    FRIEND_TEST(CPU, FUNDAMENTAL_FUNCTIONS);
};

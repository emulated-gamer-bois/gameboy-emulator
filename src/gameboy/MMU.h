//
// Created by algot on 2021-02-15.
//

#ifndef LAME_BOY_MMU_H
#define LAME_BOY_MMU_H

#include <cstdint>
#include <array>
#include <vector>
#include <string>

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
#define TIMER_DIVIDER       0xff04
#define TIMER_COUNTER       0xff05
#define TIMER_MODULO        0xff06
#define TIMER_CONTROL       0xff07

// Joypad constants
#define JOYPAD_SEL_BUTTONS      0x10
#define JOYPAD_SEL_DIRECTIONS   0x20
#define JOYPAD_RIGHT            0
#define JOYPAD_LEFT             1
#define JOYPAD_UP               2
#define JOYPAD_DOWN             3
#define JOYPAD_A                4
#define JOYPAD_B                5
#define JOYPAD_SELECT           6
#define JOYPAD_START            7

class MMU {
public:
    MMU();
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    void load_game_rom(std::string filepath);
    void load_boot_rom(std::string filepath);
    void joypad_release(uint8_t button);
    void joypad_press(uint8_t button);
    void timer_update(uint16_t cycles);

private:
    void write_GAME_ROM_ONLY_IN_TESTS(uint16_t addr, uint8_t data);
    void write_BOOT_ROM_ONLY_IN_TESTS(uint16_t addr, uint8_t data);
    void write_io(uint16_t addr, uint8_t data);
    uint8_t read_io(uint16_t addr);
    void disable_boot_rom();

    // Using vector for memory with variable size; game_rom and xram.
    std::vector<uint8_t> game_rom;
    std::vector<uint8_t> xram;

    // Using array for memory with fixed size.
    std::array<uint8_t, 256> boot_rom;
    std::array<uint8_t, 8192> vram;
    std::array<uint8_t, 8192> ram;
    std::array<uint8_t, 160> oam;
    std::array<uint8_t, 128> io;
    std::array<uint8_t, 128> hram;

    bool booting;
    uint8_t interrupt_enable;
    uint8_t interrupt_flag;
    uint8_t io_joypad_select;
    uint8_t io_joypad;
    uint16_t timer_divider;
    uint8_t timer_counter;
    uint8_t timer_modulo;
    uint8_t timer_control;

    // Tests using private stuff
    FRIEND_TEST(MMU, read_write);
    FRIEND_TEST(MMU, disable_boot_rom);
    FRIEND_TEST(CPU, Execute_NOP_Instruction);
    FRIEND_TEST(CPU, Execute_LD_SP_D16_Instruction);
    FRIEND_TEST(CPU, FUNDAMENTAL_FUNCTIONS);
};

#endif //LAME_BOY_MMU_H

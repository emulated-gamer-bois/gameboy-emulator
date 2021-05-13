#pragma once

#include "Cartridge.h"
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
#define IO_LCD_START            0xff40
#define IO_LCD_END              0xff4b
#define IO_DISABLE_BOOT_ROM     0xff50

/**
 * This class is used to pack together all the different devices that have memory mapped to an address.
 * The read and write function can be used to access all of the memory on the Game Boy.
 */
class MMU {
public:
    MMU();

    /**
     * Clear all the following memory arrays: vram, ram, oam, hram.
     * Enable all interrupts and set booting to true.
     */
    void reset();

    /**
     * Return 1 byte according to the specified address, either from memory or from a device's memory
     * @param addr memory address
     */
    uint8_t read(uint16_t addr);

    /**
     * Write data to memory or a device's memory according to addr
     * @param addr memory address where data is to be stored
     * @param data value to be stored on memory address
     */
    void write(uint16_t addr, uint8_t data);

    /**
     * Set the bits specified in bitmask in the interrupt flag
     * @param bitmask bits to be set
     */
    void raiseInterruptFlag(uint8_t bitmask);

    /**
     * Clear the bits specified in bitmask in the interrupt flag
     * @param bitmask bits to be cleared
     */
    void clearInterruptFlag(uint8_t bitmask);

    /**
     * Add references to various devices
     * @param ppu reference to ppu instance
     * @param apu reference to apu instance
     * @param joypad reference to joypad instance
     * @param timer reference to timer instance
     * @param cartridge reference to cartridge instance
     */
    void linkDevices(std::shared_ptr<PPU> ppu, std::shared_ptr<APU> apu, std::shared_ptr<Joypad> joypad, std::shared_ptr<Timer> timer, std::shared_ptr<Cartridge> cartridge);

    /**
     * Load boot rom from file specified by filepath.
     * Disable boot rom if load is not successful.
     * @param filepath filepath to boot rom file
     * @return true if boot rom is successfully loaded
     * @return false if file size is not 256 bytes or if unable to read file
     */
    bool loadBootRom(const std::string& filepath);

private:
    /**
     * Write to game rom located on cartridge.
     * Is only to be used in test.
     * @param addr memory address to cartridge rom
     * @param data data to be written on address
     */
    void write_GAME_ROM_ONLY_IN_TESTS(uint16_t addr, uint8_t data);

    /**
     * Write to boot rom.
     * Is only to be used in test.
     * @param addr memory address to boot rom
     * @param data data to be written on address
     */
    void write_BOOT_ROM_ONLY_IN_TESTS(uint16_t addr, uint8_t data);

    /**
     * Disable boot rom.
     * Sets booting to false if data != 0.
     * @param data disable boot rom if data != 0
     */
    void disableBootRom(uint8_t data);

    // Devices
    std::shared_ptr<Cartridge> cartridge;
    std::shared_ptr<Joypad> joypad;
    std::shared_ptr<Timer> timer;
    std::shared_ptr<PPU> ppu;
    std::shared_ptr<APU> apu;

    // Using array for memory with fixed size.
    std::array<uint8_t, 256> bootRom{};
    std::array<uint8_t, 8192> vram{};
    std::array<uint8_t, 8192> ram{};
    std::array<uint8_t, 160> oam{};
    std::array<uint8_t, 128> hram{};

    bool booting{};
    uint8_t interruptEnable{};
    uint8_t interruptFlag{};

    // Tests using private stuff
    FRIEND_TEST(MMU, read_write);
    FRIEND_TEST(MMU, disable_boot_rom);
    FRIEND_TEST(CPU, Execute_NOP_Instruction);
    FRIEND_TEST(CPU, Execute_LD_SP_D16_Instruction);
    FRIEND_TEST(CPU, FUNDAMENTAL_FUNCTIONS);
};

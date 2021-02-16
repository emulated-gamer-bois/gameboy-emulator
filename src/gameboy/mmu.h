//
// Created by algot on 2021-02-15.
//

#ifndef LAME_BOY_MMU_H
#define LAME_BOY_MMU_H

#include <stdint.h>
#include <array>
#include <vector>
#include <string>

#define ADDR_SPACE_START 0x0000
#define BOOT_ROM_START   0x0000
#define BOOT_ROM_END     0x00ff
#define GAME_ROM_START   0x0000
#define GAME_ROM_END     0x7fff
#define VRAM_START       0x8000
#define VRAM_END         0x9fff
#define xRAM_START       0xa000
#define xRAM_END         0xbfff
#define RAM_START        0xc000
#define RAM_END          0xdfff
#define OAM_START        0xfe00
#define OAM_END          0xfe9f
#define IO_START         0xff00
#define IO_END           0xff7f
#define HRAM_START       0xff80
#define HRAM_END         0xffff
#define ADDR_SPACE_END   0xffff

class MMU {
public:
    MMU();
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t data);
    void disable_boot_rom();
    void load_rom(std::string filepath);

private:
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
};

#endif //LAME_BOY_MMU_H

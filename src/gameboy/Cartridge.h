//
// Created by Algot on 2021-03-11.
//

#pragma once

#include "MBC.h"
#include <cstdint>
#include <string>
#include <vector>

class Cartridge {
private:
    enum CartridgeType {
        // When adding cartridge support add it to Cartridge::valid_cartridge_type()
        ROM_ONLY = 0x0,
        MBC1 = 0x1,
        MBC1_R = 0x2,
        MBC1_R_B = 0x3,
    };

    uint8_t cartridge_type;
    uint8_t rom_size;
    uint8_t ram_size;
    std::shared_ptr<std::vector<uint8_t>> rom;
    std::shared_ptr<std::vector<uint8_t>> ram;
    std::unique_ptr<MBC> mbc;

    static bool valid_cartridge_type(uint8_t cartrigde_type);
public:
    Cartridge();
    void reset();
    uint8_t read(uint16_t addr) const;
    void write(uint16_t addr, uint8_t data);
    bool load_rom(std::string filepath);
    void write_TEST(uint16_t addr, uint8_t data);
};

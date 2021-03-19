//
// Created by Algot on 2021-03-11.
//

#include "Cartridge.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <cstring> // memcpy

Cartridge::Cartridge() {
    this->reset();
}

void Cartridge::reset() {
    this->cartridge_type = 0;
    this->rom_size = 0;
    this->ram_size = 0;

    this->rom = std::make_shared<std::vector<uint8_t>>(0x8000);
    this->ram = nullptr;
    this->mbc = std::make_unique<ROM_Only_MBC>(this->rom);
}

uint8_t Cartridge::read(uint16_t addr) const {
    return this->mbc->read(addr);
}

void Cartridge::write(uint16_t addr, uint8_t data) {
    this->mbc->write(addr, data);
}

void Cartridge::write_TEST(uint16_t addr, uint8_t data) {
    this->rom->at(addr) = data;
}

bool Cartridge::load_rom(std::string filepath) {
    std::streampos size;
    char *memblock;

    std::ifstream file (filepath, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        // Get file size
        size = file.tellg();
        memblock = new char [size];

        // Move seeker to beginning of file
        file.seekg (0, std::ios::beg);
        // Read file to buffer
        file.read (memblock, size);
        // Close file
        file.close();

        this->rom->resize(size);
        // Copy data
        std::memcpy(&this->rom->at(this->rom->size() - size), &memblock[0], size);

        // Check cartridge type
        this->cartridge_type = memblock[0x147];
        if (!this->valid_cartridge_type(this->cartridge_type)) {
            std::cout << "ROM file has unsupported cartridge type: " << (int)this->cartridge_type << std::endl;
            return false;
        } else {
            switch (this->cartridge_type) {
                case ROM_ONLY:
                    this->mbc = std::make_unique<ROM_Only_MBC>(this->rom);
                    break;
                case MBC1:
                case MBC1_R:
                case MBC1_R_B:
                    this->mbc = std::make_unique<MBC1_MBC>(this->rom, this->ram);
                    break;
            }
        }

        delete[] memblock;


        // Set ROM/RAM size
        this->rom_size = this->rom->at(0x148);
        this->ram_size = this->rom->at(0x149);
    }
    else {
        std::cout << "Unable to open game ROM: " << filepath << std::endl;
        return false;
    }
    return true;
}

bool Cartridge::valid_cartridge_type(uint8_t cartrigde_type) {
    switch (cartrigde_type) {
        case ROM_ONLY:
        case MBC1:
        case MBC1_R:
        case MBC1_R_B:
            return true;
        default:
            return false;
    }
}

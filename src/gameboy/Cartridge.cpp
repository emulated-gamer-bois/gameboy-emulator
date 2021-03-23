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

    this->rom = std::vector<uint8_t>(0x8000);
    this->ram;
    this->mbc = std::make_unique<ROM_Only_MBC>(&this->rom);
}

uint8_t Cartridge::read(uint16_t addr) const {
    return this->mbc->read(addr);
}

void Cartridge::write(uint16_t addr, uint8_t data) {
    this->mbc->write(addr, data);
}

void Cartridge::write_TEST(uint16_t addr, uint8_t data) {
    this->rom.at(addr) = data;
}

bool Cartridge::load_rom(const std::string& filepath) {
    std::streampos size;

    std::ifstream file (filepath, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        // Get file size
        size = file.tellg();
        std::unique_ptr<char[]> memblock(new char[size]);

        // Move seeker to beginning of file
        file.seekg (0, std::ios::beg);
        // Read file to buffer
        file.read (memblock.get(), size);
        // Close file
        file.close();

        // Set ROM/RAM size
        this->rom_size = memblock[0x148];
        if (!this->init_rom()) return false;

        this->ram_size = memblock[0x149];
        if (!this->init_ram()) return false;

        // Check cartridge type
        this->cartridge_type = memblock[0x147];
        if (!this->init_mbc()) return false;

        // Copy data
        std::memcpy(&this->rom.at(this->rom.size() - size), &memblock[0], size);

        this->filepath = filepath;
        return true;
    }
    else {
        std::cout << "Unable to open game ROM: " << filepath << std::endl;
        return false;
    }
}

bool Cartridge::save_ram() {
    // Return if Cartridge have no RAM
    if (this->ram_size == Cartridge::RAM_NO_RAM) {
        return true;
    }
    // Create file
    std::ofstream wfile (this->filepath + ".sav", std::ios::out|std::ios::binary|std::ios::ate);
    if(!wfile) {
        std::cout << "Cannot open file!" << std::endl;
        return false;
    }
    // Write to file
    wfile.write(reinterpret_cast<const char *>(this->ram.data()), this->ram.size());
    wfile.close();

    if(!wfile.good()) {
        std::cout << "Error occurred at writing time!" << std::endl;
        return false;
    }
    return true;
}

bool Cartridge::init_rom() {
    switch (this->rom_size)
    {
        case ROM_32KB:
            this->rom = std::vector<uint8_t>(0x8000);
            break;
        case ROM_64KB:
            this->rom = std::vector<uint8_t>(0x10000);
            break;
        case ROM_128KB:
            this->rom = std::vector<uint8_t>(0x20000);
            break;
        case ROM_256KB:
            this->rom = std::vector<uint8_t>(0x40000);
            break;
        case ROM_512KB:
            this->rom = std::vector<uint8_t>(0x80000);
            break;
        case ROM_1MB:
            this->rom = std::vector<uint8_t>(0x100000);
            break;
        case ROM_2MB:
            this->rom = std::vector<uint8_t>(0x200000);
            break;
        case ROM_4MB:
            this->rom = std::vector<uint8_t>(0x400000);
            break;
        case ROM_8MB:
            this->rom = std::vector<uint8_t>(0x800000);
            break;

        default:
            std::cout << "Invalid or unsupported ROM size: " << (int)this->rom_size << std::endl;
            return false;
    }
    std::cout << "ROM size: " << (int)this->rom_size << std::endl;
    return true;
}

bool Cartridge::init_ram() {
    switch (this->ram_size)
    {
        case RAM_NO_RAM:
            // Should be empty ram, but for security add accessible memory
        case RAM_8KB:
            this->ram = std::vector<uint8_t>(0x2000);
            break;
        case RAM_32KB:
            this->ram = std::vector<uint8_t>(0x8000);
            break;
        case RAM_128KB:
            this->ram = std::vector<uint8_t>(0x20000);
            break;
        case RAM_64KB:
            this->ram = std::vector<uint8_t>(0x10000);
            break;

        default:
            std::cout << "Invalid or unsupported RAM size: "<< (int)this->ram_size << std::endl;
            return false;
    }
    std::cout << "RAM size: "<< (int)this->ram_size << std::endl;
    return true;
}

bool Cartridge::init_mbc() {
    switch (this->cartridge_type) {
        case ROM_ONLY:
            this->mbc = std::make_unique<ROM_Only_MBC>(&this->rom);
            break;
        case MBC1:
        case MBC1_R:
        case MBC1_R_B:
            this->mbc = std::make_unique<MBC1_MBC>(&this->rom, &this->ram);
            break;
        case MBC3_T_B:
        case MBC3_T_R_B:
        case MBC3:
        case MBC_R:
        case MBC_R_B:
            this->mbc = std::make_unique<MBC3_MBC>(&this->rom, &this->ram);
            break;

        default:
            std::cout << "ROM file has unsupported cartridge type: " << (int)this->cartridge_type << std::endl;
            return false;
    }
    std::cout << "Cartridge type: " << (int)this->cartridge_type << std::endl;
    return true;
}

void Cartridge::update(uint8_t cycles) {
    this->mbc->update(cycles);
}

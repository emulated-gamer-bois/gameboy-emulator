//
// Created by Algot on 2021-03-11.
//

#include "Cartridge.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <cstring> // memcpy

Cartridge::Cartridge() {
    reset();
}

void Cartridge::reset() {
    cartridgeType = 0;
    romSize = 0;
    ramSize = 0;

    rom = std::vector<uint8_t>(0x8000);
    ram;
    mbc = std::make_unique<ROM_Only_MBC>(&rom);
}

uint8_t Cartridge::read(uint16_t addr) const {
    return mbc->read(addr);
}

void Cartridge::write(uint16_t addr, uint8_t data) {
    mbc->write(addr, data);
}

void Cartridge::writeTest(uint16_t addr, uint8_t data) {
    rom.at(addr) = data;
}

bool Cartridge::loadRom(const std::string& filepath, bool load_ram_from_file) {
    this->filepath = filepath;
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
        romSize = memblock[0x148];
        if (!initRom()) return false;

        ramSize = memblock[0x149];
        if (!initRam(load_ram_from_file)) return false;

        // Check cartridge type
        cartridgeType = memblock[0x147];
        if (!initMbc()) return false;

        // Copy data
        std::memcpy(&rom.at(rom.size() - size), &memblock[0], size);

        return true;
    }
    else {
        std::cout << "Unable to open game ROM: " << filepath << std::endl;
        return false;
    }
}

bool Cartridge::saveRam() {
    // Return if Cartridge have no RAM
    if (ramSize == Cartridge::RAM_NO_RAM) {
        return true;
    }
    // Create file
    std::ofstream wfile (filepath + ".sav", std::ios::out|std::ios::binary|std::ios::ate);
    if(!wfile) {
        std::cout << "Cannot open file!" << std::endl;
        return false;
    }
    // Write to file
    wfile.write(reinterpret_cast<const char *>(ram.data()), ram.size());
    wfile.close();

    if(!wfile.good()) {
        std::cout << "Error occurred at writing time!" << std::endl;
        return false;
    }
    return true;
}

bool Cartridge::loadRam() {
    // Return if Cartridge should not have RAM
    if (ramSize == Cartridge::RAM_NO_RAM) {
        return true;
    }

    std::streampos size;
    std::ifstream rfile (filepath + ".sav", std::ios::in|std::ios::binary|std::ios::ate);
    if (rfile.is_open())
    {
        // Get file size
        size = rfile.tellg();

        if (size != ram.size()) {
            std::cout << "Savefile: " << filepath + ".sav" << " has wrong file size" << std::endl;
            return false;
        }

        // Move seeker to beginning of file
        rfile.seekg (0, std::ios::beg);
        // Read file to ram
        rfile.read (reinterpret_cast<char *>(ram.data()), size);
        // Close file
        rfile.close();

        return true;
    }
    else {
        std::cout << "Unable to open file: " << filepath << std::endl;
        return false;
    }
}

bool Cartridge::initRom() {
    switch (romSize)
    {
        case ROM_32KB:
            rom = std::vector<uint8_t>(0x8000);
            break;
        case ROM_64KB:
            rom = std::vector<uint8_t>(0x10000);
            break;
        case ROM_128KB:
            rom = std::vector<uint8_t>(0x20000);
            break;
        case ROM_256KB:
            rom = std::vector<uint8_t>(0x40000);
            break;
        case ROM_512KB:
            rom = std::vector<uint8_t>(0x80000);
            break;
        case ROM_1MB:
            rom = std::vector<uint8_t>(0x100000);
            break;
        case ROM_2MB:
            rom = std::vector<uint8_t>(0x200000);
            break;
        case ROM_4MB:
            rom = std::vector<uint8_t>(0x400000);
            break;
        case ROM_8MB:
            rom = std::vector<uint8_t>(0x800000);
            break;

        default:
            std::cout << "Invalid or unsupported ROM size: " << (int)romSize << std::endl;
            return false;
    }
    std::cout << "ROM size: " << (int)romSize << std::endl;
    return true;
}

bool Cartridge::initRam(bool loadFromRam) {
    switch (ramSize)
    {
        case RAM_NO_RAM:
            // Should be empty ram, but for security add accessible memory
        case RAM_8KB:
            ram = std::vector<uint8_t>(0x2000);
            break;
        case RAM_32KB:
            ram = std::vector<uint8_t>(0x8000);
            break;
        case RAM_128KB:
            ram = std::vector<uint8_t>(0x20000);
            break;
        case RAM_64KB:
            ram = std::vector<uint8_t>(0x10000);
            break;

        default:
            std::cout << "Invalid or unsupported RAM size: "<< (int)ramSize << std::endl;
            return false;
    }
    if (loadFromRam) {
        if (!loadRam()) {
            std::cout << "Could not load extended RAM from file!" << std::endl;
        } else {
            std::cout << "Extended RAM loaded from file!" << std::endl;
        }
    }
    std::cout << "RAM size: "<< (int)ramSize << std::endl;
    return true;
}

bool Cartridge::initMbc() {
    switch (cartridgeType) {
        case ROM_ONLY:
            mbc = std::make_unique<ROM_Only_MBC>(&rom);
            break;
        case MBC1:
        case MBC1_R:
        case MBC1_R_B:
            mbc = std::make_unique<MBC1_MBC>(&rom, &ram);
            break;
        case MBC3_T_B:
        case MBC3_T_R_B:
        case MBC3:
        case MBC_R:
        case MBC_R_B:
            mbc = std::make_unique<MBC3_MBC>(&rom, &ram);
            break;

        default:
            std::cout << "ROM file has unsupported cartridge type: " << (int)cartridgeType << std::endl;
            return false;
    }
    std::cout << "Cartridge type: " << (int)cartridgeType << std::endl;
    return true;
}

void Cartridge::update(uint8_t cycles) {
    mbc->update(cycles);
}

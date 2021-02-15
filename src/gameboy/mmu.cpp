//
// Created by algot on 2021-02-15.
//

#include "mmu.h"
#include <iostream>
#include <fstream>
#include <string>

MMU::MMU() {
    // Reset arrays to 0
    this->boot_rom.fill(0x00);
    this->vram.fill(0x00);
    this->ram.fill(0x00);
    this->oam.fill(0x00);
    this->io.fill(0x00);
    this->hram.fill(0x00);

    this->game_rom.resize(0x8000);

    //this->xram.resize(0x4000);              // Not used in simple games.
                                              // Size depends on info in game rom header.

    this->booting = true;
}

uint8_t MMU::read(uint16_t addr) {
    // Temporary ugly if statements
    if (GAME_ROM_START <= addr && addr <= GAME_ROM_END) {
        return this->game_rom[addr];
    } else {
        return 0x00;
    }
}

void MMU::write(uint16_t addr, uint8_t data) {
    // Temporary ugly if statements
    if (GAME_ROM_START <= addr && addr <= GAME_ROM_END) {
        this->game_rom[addr] = data;
    }
}

void MMU::disable_boot_rom() {
    this->booting = false;
}

void MMU::load_rom(std::string filepath) {
    std::streampos size;
    char *memblock;
    std::string prefix = "..\\..\\roms\\";

    std::ifstream file (prefix.append(filepath), std::ios::in|std::ios::binary|std::ios::ate);
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

        // Resize game_rom to size
        this->game_rom.resize(size);
        // Copy data
        std::memcpy(&this->game_rom[this->game_rom.size() - size], &memblock[0], size);

        delete[] memblock;
    }
    else {
        std::cout << "Unable to open file";
        exit(1);
    }
}

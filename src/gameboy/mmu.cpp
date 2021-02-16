//
// Created by algot on 2021-02-15.
//

#include "mmu.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>


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
    switch(addr & 0xe000) {
            //Boot ROM/Game ROM
        case GAME_ROM_START:
            if(BOOT_ROM_START <= addr && addr <= BOOT_ROM_END && this->booting) {
                return this->boot_rom[addr];
            }
        case GAME_ROM_START + 0x2000:
        case GAME_ROM_START + 0x4000:
        case GAME_ROM_START + 0x6000:
            return this->game_rom[addr];

            //Video RAM
        case VRAM_START:
            return this->vram[addr - VRAM_START];

            //External RAM
        case xRAM_START:
            return this->xram[addr - xRAM_START];

            //Work RAM
        case RAM_START:
            return this->ram[addr - RAM_START];

            //OAM / IO / High RAM
        case 0xe000:
            if(OAM_START <= addr && addr <= OAM_END) {
                return this->oam[addr - OAM_START];
            } else if (IO_START <= addr && addr <= IO_END) {
                return this->io[addr - IO_START];
            } else if (HRAM_START <= addr && addr <= HRAM_END) {
                return this->hram[addr - HRAM_START];
            }

        default:
            std::cout<<"Tried to read unused memory address: "<<addr;
            exit(1);
    }
}

void MMU::write(uint16_t addr, uint8_t data) {
    switch(addr & 0xe000) {
            //Boot ROM/Game ROM
        case GAME_ROM_START:
        case GAME_ROM_START + 0x2000:
        case GAME_ROM_START + 0x4000:
        case GAME_ROM_START + 0x6000:
            std::cout<<"Tried to write to ROM at address: "<<addr;
            exit(1); //TODO: could write to ROM address when bankswitching

            //Video RAM
        case VRAM_START:
            this->vram[addr - VRAM_START] = data;

            //External RAM
        case xRAM_START:
            this->xram[addr - xRAM_START] = data;

            //Work RAM
        case RAM_START:
            this->ram[addr - RAM_START] = data;

            //OAM / IO / High RAM
        case 0xe000:
            if(OAM_START <= addr && addr <= OAM_END) {
                this->oam[addr - OAM_START] = data;
            } else if (IO_START <= addr && addr <= IO_END) {
                this->io[addr - IO_START] = data;
            } else if (HRAM_START <= addr && addr <= HRAM_END) {
                this->hram[addr - HRAM_START] = data;
            }

        default:
            std::cout<<"Tried to write to unused memory address: "<<addr;
            exit(1);
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

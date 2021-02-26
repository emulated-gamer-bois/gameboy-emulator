//
// Created by algot on 2021-02-15.
//

#include "MMU.h"
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
    // Enable all interrupts by default
    this->interrupt_enable = 0b11111;
    // No interrupt requests by default
    this->interrupt_flag = 0;

    // No buttons pressed by default
    this->io_joypad = 0xff;

    // Timer default values
    timer_divider = 0;
    timer_counter = 0;
    timer_modulo = 0;
    timer_control = 0;
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
                return this->read_io(addr);
            } else if (HRAM_START <= addr && addr <= HRAM_END) {
                return this->hram[addr - HRAM_START];
            } else if (addr == INTERRUPT_ENABLE) {
                return this->interrupt_enable;
            }

        default:
            std::cout << "Tried to read unused memory address: " << addr << std::endl;
            // TODO: Error handling
    }
    return 0;
}

void MMU::write(uint16_t addr, uint8_t data) {
    switch(addr & 0xe000) {
            //Boot ROM/Game ROM
        case GAME_ROM_START:
        case GAME_ROM_START + 0x2000:
        case GAME_ROM_START + 0x4000:
        case GAME_ROM_START + 0x6000:
            std::cout << "Tried to write to ROM at address: " << addr << std::endl;
            // TODO: Error handling
            // TODO: could write to ROM addresses when bankswitching is implemented

            //Video RAM
        case VRAM_START:
            this->vram[addr - VRAM_START] = data;
            break;

            //External RAM
        case xRAM_START:
            this->xram[addr - xRAM_START] = data;
            break;

            //Work RAM
        case RAM_START:
            this->ram[addr - RAM_START] = data;
            break;

            //OAM / IO / High RAM
        case 0xe000:
            if(OAM_START <= addr && addr <= OAM_END) {
                this->oam[addr - OAM_START] = data;
                break;
            } else if (IO_START <= addr && addr <= IO_END) {
                this->write_io(addr, data);
                break;
            } else if (HRAM_START <= addr && addr <= HRAM_END) {
                this->hram[addr - HRAM_START] = data;
                break;
            } else if (addr == INTERRUPT_ENABLE) {
                this->interrupt_enable = data;
                break;
            }

        default:
            std::cout << "Tried to write to unused memory address: " << addr << std::endl;
            // TODO: Error handling
    }
}

void MMU::disable_boot_rom() {
    this->booting = false;
}

void MMU::load_game_rom(std::string filepath) {
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

        // Resize game_rom to size
        this->game_rom.resize(size);
        // Copy data
        std::memcpy(&this->game_rom[this->game_rom.size() - size], &memblock[0], size);

        delete[] memblock;
    }
    else {
        std::cout << "Unable to open file: " << filepath << std::endl;
        // TODO: Error handling
    }
}

void MMU::load_boot_rom(std::string filepath) {
    std::streampos size;
    char *memblock;

    std::ifstream file (filepath, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        // Get file size
        size = file.tellg();
        if ((int)size == 256) {
            memblock = new char [size];

            // Move seeker to beginning of file
            file.seekg (0, std::ios::beg);
            // Read file to buffer
            file.read (memblock, size);
            // Close file
            file.close();

            // Copy data
            std::memcpy(&this->boot_rom[this->boot_rom.size() - size], &memblock[0], size);

            delete[] memblock;
        } else {
            std::cout << "Unable to load boot ROM! File size is not 256 bytes!" << std::endl;
            // TODO: Error handling
        }
    }
    else {
        std::cout << "Unable to open file: " << filepath << std::endl;
        // TODO: Error handling
    }
}

// ONLY TO BE USED IN TESTS. CAN WRITE TO GAME ROM
void MMU::write_GAME_ROM_ONLY_IN_TESTS(uint16_t addr, uint8_t data) {
    if (GAME_ROM_START <= addr && addr <= GAME_ROM_END) {
        this->game_rom[addr] = data;
    } else {
        std::cout << "Tried to use write_GAME_ROM_ONLY_IN_TESTS with invalid addr: " << addr << std::endl;
        // TODO: Error handling
    }
}

// ONLY TO BE USED IN TESTS. CAN WRITE TO BOOT ROM
void MMU::write_BOOT_ROM_ONLY_IN_TESTS(uint16_t addr, uint8_t data) {
    if (BOOT_ROM_START <= addr && addr <= BOOT_ROM_END) {
        this->boot_rom[addr] = data;
    } else {
        std::cout << "Tried to use write_BOOT_ROM_ONLY_IN_TESTS with invalid addr: " << addr << std::endl;
        // TODO: Error handling
    }
}

void MMU::write_io(uint16_t addr, uint8_t data) {
    if (addr == IO_DISABLE_BOOT_ROM) {
        if (data != 0) this->disable_boot_rom();
    } else if (addr == IO_JOYPAD) {
        this->io_joypad_select = data;
    } else if (addr == INTERRUPT_FLAG) {
        this->interrupt_flag = data;
    } else if (TIMER_DIVIDER <= addr && addr <= TIMER_CONTROL) {
        switch (addr) {
            case TIMER_DIVIDER:
                this->timer_divider = 0;
                break;
            case TIMER_COUNTER:
                this->timer_counter = ((uint16_t)data << 8);
                break;
            case TIMER_MODULO:
                this->timer_modulo = data;
                break;
            case TIMER_CONTROL:
                this->timer_control = data & 0b111;
                break;
        }
    } else {
        this->io[addr - IO_START] = data;
    }
}

uint8_t MMU::read_io(uint16_t addr) {
    if (addr == IO_JOYPAD) {
        if ((this->io_joypad_select & JOYPAD_SEL_DIRECTIONS)) {
            return ((this->io_joypad >> 0) & 0xf);
        } else {
            return ((this->io_joypad >> 4) & 0xf);
        }
    } else if (addr == INTERRUPT_FLAG) {
        return this->interrupt_flag & (0x1f);
    } else if (TIMER_DIVIDER <= addr && addr <= TIMER_CONTROL) {
        switch (addr) {
            case TIMER_DIVIDER:
                return (uint8_t)(this->timer_divider >> 8);
            case TIMER_COUNTER:
                return this->timer_counter;
            case TIMER_MODULO:
                return this->timer_modulo;
            case TIMER_CONTROL:
                return this->timer_control & 0b111;
        }
    } else {
        return this->io[addr - IO_START];
    }
    return 0;
}

// Timer functions
// Cycles is @ 1,048,576Hz
void MMU::timer_update(uint16_t cycles) {
    // Timer activated
    if (this->timer_control & (1 << 0)) {
        // Increase divider
        uint16_t old_divider = this->timer_divider;
        this->timer_divider += (cycles*4);

        // Timer speed
        uint8_t speed = this->timer_control & 0b11;
        const uint16_t speed_mask[] = {0xfc00, 0xfff0, 0xffc0, 0xff00};
        const uint8_t speed_offs[] = {10, 4, 6, 8};

        uint16_t mask = speed_mask[speed];
        uint8_t offs = speed_offs[speed];

        // Check for counter overflow
        // Increase counter
        uint8_t increase_counter = (((this->timer_divider & mask) - (old_divider & mask)) >> offs);
        if ((uint16_t)(this->timer_counter + increase_counter) > 0xff) {
            // Add modulo
            this->timer_counter += (this->timer_modulo + increase_counter);
            // Raise interrupt request for Timer interrupt
            this->interrupt_flag = this->interrupt_flag | (1 << 2);
        } else {
          this->timer_counter += increase_counter;
        }
    }
}

// Joypad functions
void MMU::joypad_release(uint8_t button) {
    auto temp = this->io_joypad;
    this->io_joypad = temp | (1 << button);
}

void MMU::joypad_press(uint8_t button) {
    uint8_t temp = this->io_joypad;
    this->io_joypad = (temp & ~(1 << button));

    // Raise interrupt flag
    temp = this->interrupt_flag;
    this->interrupt_flag = (temp | (1 << 4));
}

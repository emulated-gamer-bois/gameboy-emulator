//
// Created by algot on 2021-02-15.
//

#include "MMU.h"
#include "Joypad.h"
#include "Timer.h"
#include "PPU.h"

#include <memory> // smart pointers
#include <iostream> // cout
#include <fstream>
#include <cstring> // memcpy

MMU::MMU() {
    this->reset();
}

void MMU::link_devices(std::shared_ptr<PPU> ppu, std::shared_ptr<Joypad> joypad, std::shared_ptr<Timer> timer) {
    if (ppu) {
        this->ppu = ppu;
    }
    if (joypad) {
        this->joypad = joypad;
    }
    if (timer) {
        this->timer = timer;
    }
}

void MMU::reset() {
    // Reset arrays to 0
    this->boot_rom.fill(0x00);
    this->vram.fill(0x00);
    this->ram.fill(0x00);
    this->oam.fill(0x00);
    this->hram.fill(0x00);

    this->booting = true;

    // Enable all interrupts by default
    this->interrupt_enable = 0b11111;
    // No interrupt requests by default
    this->interrupt_flag = 0;

    this->cartridge = std::make_unique<Cartridge>();
}

uint8_t MMU::read(uint16_t addr) {
    // Boot ROM / Cartridge
    if (GAME_ROM_START <= addr && addr <= GAME_ROM_END) {
        if (BOOT_ROM_START <= addr && addr <= BOOT_ROM_END && booting) {
            return this->boot_rom[addr];
        } else {
            return this->cartridge->read(addr);
        }
    }

    // VRAM
    if (VRAM_START <= addr && addr <= VRAM_END) {
        return this->vram[addr - VRAM_START];
    }

    // xRAM
    if (xRAM_START <= addr && addr <= xRAM_END) {
        return this->cartridge->read(addr);
    }

    // WRAM
    if (WRAM_START <= addr && addr <= WRAM_END) {
        return this->ram[addr - WRAM_START];
    }

    // OAM
    if (OAM_START <= addr && addr <= OAM_END) {
        return this->oam[addr - OAM_START];
    }

    // I/O
    if (IO_START <= addr && addr <= IO_END) {
        // Joypad
        if (addr == IO_JOYPAD) {
            return joypad->read(addr);
        }

        // Serial Data Transfer TODO: Implement Serial Data Transfer
        if (IO_SERIAL_DATA_START <= addr && addr <= IO_SERIAL_DATA_END) {
            // Supress to prevent console spam
            // std::cout << "Tried to read from Serial Data Transfer device. addr: " << (int)addr << std::endl;
            return 0;
        }

        // Timer
        if (IO_TIMER_START <= addr && addr <= IO_TIMER_END) {
            return this->timer->read(addr);
        }

        // Interrupt Flag
        if (addr == INTERRUPT_FLAG) {
            return this->interrupt_flag;
        }

        // Sound TODO: Implement sound
        if (IO_SOUND_START <= addr && addr <= IO_SOUND_END) {
            // Supress to prevent console spam
            // std::cout << "Tried to read from sound device. addr: " << (int)addr << std::endl;
            return 0;
        }

        // Waveform RAM TODO: Implement sound
        if (IO_WAVEFORM_RAM_START <= addr && addr <= IO_WAVEFORM_RAM_END) {
            // Supress to prevent console spam
            // std::cout << "Tried to read Waveform RAM. addr: " << (int)addr << std::endl;
            return 0;
        }

        // LCD
        if (IO_LCD_START <= addr && addr <= IO_LCD_END) {
            return this->ppu->read(addr);
        }
    }

    // HRAM
    if (HRAM_START <= addr && addr <= HRAM_END) {
        return this->hram[addr - HRAM_START];
    }

    // Interrupt Enable
    if (addr == INTERRUPT_ENABLE) {
        return this->interrupt_enable;
    }

    // Address did not match
    std::cout << "Tried to read unmapped memory address: " << (int)addr << std::endl;
    return 0;
}

void MMU::write(uint16_t addr, uint8_t data) {
    // Memory Bank Controller
    if (BOOT_ROM_START <= addr && addr <= BOOT_ROM_END) {
        this->cartridge->write(addr, data);
        return;
    }

    // VRAM
    if (VRAM_START <= addr && addr <= VRAM_END) {
        this->vram[addr - VRAM_START] = data;
        return;
    }

    // xRAM
    if (xRAM_START <= addr && addr <= xRAM_END) {
        this->cartridge->write(addr, data);
        return;
    }

    // WRAM
    if (WRAM_START <= addr && addr <= WRAM_END) {
        this->ram[addr - WRAM_START] = data;
        return;
    }

    // OAM
    if (OAM_START <= addr && addr <= OAM_END) {
        this->oam[addr - OAM_START] = data;
        return;
    }

    // IO
    if (IO_START <= addr && addr <= IO_END) {
        // Joypad
        if (addr == IO_JOYPAD) {
            this->joypad->write(addr, data);
            return;
        }

        // Serial Data Transfer TODO: Implement Serial Data Transfer
        if (IO_SERIAL_DATA_START <= addr && addr <= IO_SERIAL_DATA_END) {
            // Supress to prevent console spam
            // std::cout << "Tried to write to Serial Data Transfer device. addr: " << (int)addr << " data: " << (int)data << std::endl;
            return;
        }

        // Timer
        if (IO_TIMER_START <= addr && addr <= IO_TIMER_END) {
            this->timer->write(addr, data);
            return;
        }

        // Interrupt Flag
        if (addr == INTERRUPT_FLAG) {
            this->interrupt_flag = data;
            return;
        }

        // Sound TODO: Implement sound
        if (IO_SOUND_START <= addr && addr <= IO_SOUND_END) {
            // Supress to prevent console spam
            // std::cout << "Tried to write to sound device. addr: " << (int)addr << " data: " << (int)data << std::endl;
            return;
        }

        // Waveform RAM TODO: Implement sound
        if (IO_WAVEFORM_RAM_START <= addr && addr <= IO_WAVEFORM_RAM_END) {
            // Supress to prevent console spam
            // std::cout << "Tried to write to Waveform RAM. addr: " << (int)addr << " data: " << (int)data<< std::endl;
            return;
        }

        // LCD
        if (IO_LCD_START <= addr && addr <= IO_LCD_END) {
            this->ppu->write(addr, data);
            return;
        }

        // Disable boot ROM
        if (addr == IO_DISABLE_BOOT_ROM) {
            this->disable_boot_rom(data);
            return;
        }

        std::cout << "Tried to write to unmapped memory address: " << (int)addr << " data: " << (int)data << std::endl;
    }

    // HRAM
    if (HRAM_START <= addr && addr <= HRAM_END) {
        this->hram[addr - HRAM_START] = data;
        return;
    }

    // Interrupt Enable
    if (addr == INTERRUPT_ENABLE) {
        this->interrupt_enable = data;
        return;
    }

    // Address did not match
    std::cout << "Tried to write to unmapped memory address: " << (int)addr << " data: " << (int)data << std::endl;
}

void MMU::disable_boot_rom(uint8_t data) {
    if (data != 0) {
        this->booting = false;
    }
}

bool MMU::load_game_rom(std::string filepath) {
    return this->cartridge->load_rom(filepath);
}

bool MMU::load_boot_rom(std::string filepath) {
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
            disable_boot_rom(1);
            return false;
        }
    }
    else {
        std::cout << "Unable to open boot ROM: " << filepath << std::endl;
        disable_boot_rom(1);
        return false;
    }
    return true;
}

// ONLY TO BE USED IN TESTS. CAN WRITE TO GAME ROM
void MMU::write_GAME_ROM_ONLY_IN_TESTS(uint16_t addr, uint8_t data) {
    if (GAME_ROM_START <= addr && addr <= GAME_ROM_END) {
        this->cartridge->write_TEST(addr, data);
    } else {
        std::cout << "Tried to use write_GAME_ROM_ONLY_IN_TESTS with invalid addr: " << addr << std::endl;
    }
}

// ONLY TO BE USED IN TESTS. CAN WRITE TO BOOT ROM
void MMU::write_BOOT_ROM_ONLY_IN_TESTS(uint16_t addr, uint8_t data) {
    if (BOOT_ROM_START <= addr && addr <= BOOT_ROM_END) {
        this->boot_rom[addr] = data;
    } else {
        std::cout << "Tried to use write_BOOT_ROM_ONLY_IN_TESTS with invalid addr: " << addr << std::endl;
    }
}

void MMU::raise_interrupt_flag(uint8_t bitmask) {
    this->interrupt_flag |= bitmask;
}

void MMU::clear_interrupt_flag(uint8_t bitmask) {
    this->interrupt_flag &= ~bitmask;
}

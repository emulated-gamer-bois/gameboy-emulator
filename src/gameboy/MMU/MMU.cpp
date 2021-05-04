#include "MMU.h"
#include "../Joypad.h"
#include "../PPU/PPU.h"
#include "Timer.h"

#include <cstring> // memcpy
#include <iostream> // cout
#include <memory>   // smart pointers

MMU::MMU() {
    reset();
}
void MMU::linkDevices(std::shared_ptr<PPU> ppu, std::shared_ptr<APU> apu, std::shared_ptr<Joypad> joypad, std::shared_ptr<Timer> timer, std::shared_ptr<Cartridge> cartridge) {
    if (ppu) {
        this->ppu = ppu;
    }
    if (apu) {
        this->apu = apu;
    }
    if (joypad) {
        this->joypad = joypad;
    }
    if (timer) {
        this->timer = timer;
    }
    if (cartridge) {
        this->cartridge = cartridge;
    }
}

void MMU::reset() {
    // Reset arrays to 0
    bootRom.fill(0x00);
    vram.fill(0x00);
    ram.fill(0x00);
    oam.fill(0x00);
    hram.fill(0x00);

    booting = true;

    // Enable all interrupts by default
    interruptEnable = 0b11111;
    // No interrupt requests by default
    interruptFlag = 0;
}

uint8_t MMU::read(uint16_t addr) {
    // Boot ROM / Cartridge
    if (GAME_ROM_START <= addr && addr <= GAME_ROM_END) {
        if (BOOT_ROM_START <= addr && addr <= BOOT_ROM_END && booting) {
            return bootRom[addr];
        } else {
            return cartridge->read(addr);
        }
    }

    // VRAM
    if (VRAM_START <= addr && addr <= VRAM_END) {
        return vram[addr - VRAM_START];
    }

    // xRAM
    if (xRAM_START <= addr && addr <= xRAM_END) {
        return cartridge->read(addr);
    }

    // WRAM
    if (WRAM_START <= addr && addr <= WRAM_END) {
        return ram[addr - WRAM_START];
    }

    //ECHO RAM
    if (ECHO_RAM_START <= addr && addr <= ECHO_RAM_END) {
        //TODO What is intended behaviour?
        std::cout << "Tried to read from echo RAM. addr: " << (int)addr << std::endl;
        return 0;
    }

    // OAM
    if (OAM_START <= addr && addr <= OAM_END) {
        return oam[addr - OAM_START];
    }

    // PROHIBITED
    if (PROHIBITED_START <= addr && addr <= PROHIBITED_END) {
        std::cout << "Tried to read from prohibited memory area. addr: " << (int)addr << std::endl;
        return 0;
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
            return timer->read(addr);
        }

        // Interrupt Flag
        if (addr == INTERRUPT_FLAG) {
            return interruptFlag;
        }

        // Sound
        if (IO_SOUND_START <= addr && addr <= IO_SOUND_END) {
            return apu->read(addr);
        }



        // LCD
        if (IO_LCD_START <= addr && addr <= IO_LCD_END) {
            return ppu->read(addr);
        }
    }

    // HRAM
    if (HRAM_START <= addr && addr <= HRAM_END) {
        return hram[addr - HRAM_START];
    }

    // Interrupt Enable
    if (addr == INTERRUPT_ENABLE) {
        return interruptEnable;
    }

    // Address did not match
    //std::cout << "Tried to read unmapped memory address: " << (int)addr << std::endl;
    return 0;
}

void MMU::write(uint16_t addr, uint8_t data) {
    // Memory Bank Controller
    if (GAME_ROM_START <= addr && addr <= GAME_ROM_END) {
        cartridge->write(addr, data);
        return;
    }

    // VRAM
    if (VRAM_START <= addr && addr <= VRAM_END) {
        vram[addr - VRAM_START] = data;
        return;
    }

    // xRAM
    if (xRAM_START <= addr && addr <= xRAM_END) {
        cartridge->write(addr, data);
        return;
    }

    // WRAM
    if (WRAM_START <= addr && addr <= WRAM_END) {
        ram[addr - WRAM_START] = data;
        return;
    }

    //ECHO RAM
    if (ECHO_RAM_START <= addr && addr <= ECHO_RAM_END) {
        //TODO What is intended behaviour?
        std::cout << "Tried to write to echo RAM. addr: " << (int)addr << " data: " << (int)data << std::endl;
        return;
    }

    // OAM
    if (OAM_START <= addr && addr <= OAM_END) {
        oam[addr - OAM_START] = data;
        return;
    }

    // PROHIBITED
    if (PROHIBITED_START <= addr && addr <= PROHIBITED_END) {
        std::cout << "Tried to write to prohibited memory area. addr: " << (int)addr << " data: " << (int)data << std::endl;
        return;
    }

    // IO
    if (IO_START <= addr && addr <= IO_END) {
        // Joypad
        if (addr == IO_JOYPAD) {
            joypad->write(addr, data);
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
            timer->write(addr, data);
            return;
        }

        // Interrupt Flag
        if (addr == INTERRUPT_FLAG) {
            interruptFlag = data;
            return;
        }

        // Sound
        if (IO_SOUND_START <= addr && addr <= IO_SOUND_END) {
            apu->write(addr, data);
            return;
        }



        // LCD
        if (IO_LCD_START <= addr && addr <= IO_LCD_END) {
            ppu->write(addr, data);
            return;
        }

        // Disable boot ROM
        if (addr == IO_DISABLE_BOOT_ROM) {
            disableBootRom(data);
            return;
        }

        //std::cout << "Tried to write to unmapped memory address: " << (int)addr << " data: " << (int)data << std::endl;
    }

    // HRAM
    if (HRAM_START <= addr && addr <= HRAM_END) {
        hram[addr - HRAM_START] = data;
        return;
    }

    // Interrupt Enable
    if (addr == INTERRUPT_ENABLE) {
        interruptEnable = data;
        return;
    }

    // Address did not match
    //std::cout << "Tried to write to unmapped memory address: " << (int)addr << " data: " << (int)data << std::endl;
}

void MMU::disableBootRom(uint8_t data) {
    if (data != 0) {
        booting = false;
    }
}

bool MMU::loadBootRom(const std::string& filepath) {
    std::streampos size;

    std::ifstream file(filepath, std::ios::in | std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        // Get file size
        size = file.tellg();
        if (static_cast<int>(size) == 256) {
            std::unique_ptr<char[]> memblock(new char[size]);

            // Move seeker to beginning of file
            file.seekg(0, std::ios::beg);
            // Read file to buffer
            file.read(memblock.get(), size);
            // Close file
            file.close();

            // Copy data
            std::memcpy(&bootRom[bootRom.size() - size], &memblock[0], size);
        } else {
            //std::cout << "Unable to load boot ROM! File size is not 256 bytes!" << std::endl;
            disableBootRom(1);
            return false;
        }
    } else {
        //std::cout << "Unable to open boot ROM: " << filepath << std::endl;
        disableBootRom(1);
        return false;
    }
    return true;
}

// ONLY TO BE USED IN TESTS. CAN WRITE TO GAME ROM
void MMU::write_GAME_ROM_ONLY_IN_TESTS(uint16_t addr, uint8_t data) {
    if (GAME_ROM_START <= addr && addr <= GAME_ROM_END) {
        cartridge->writeTest(addr, data);
    } else {
        //std::cout << "Tried to use write_GAME_ROM_ONLY_IN_TESTS with invalid addr: " << addr << std::endl;
    }
}

// ONLY TO BE USED IN TESTS. CAN WRITE TO BOOT ROM
void MMU::write_BOOT_ROM_ONLY_IN_TESTS(uint16_t addr, uint8_t data) {
    if (BOOT_ROM_START <= addr && addr <= BOOT_ROM_END) {
        bootRom[addr] = data;
    } else {
        //std::cout << "Tried to use write_BOOT_ROM_ONLY_IN_TESTS with invalid addr: " << addr << std::endl;
    }
}

void MMU::raiseInterruptFlag(uint8_t bitmask) {
    interruptFlag |= bitmask;
}

void MMU::clearInterruptFlag(uint8_t bitmask) {
    interruptFlag &= ~bitmask;
}

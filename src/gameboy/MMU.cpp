//
// Created by algot on 2021-02-15.
//

#include "MMU.h"
#include "Joypad.h"
#include "Timer.h"
#include "PPU.h"

#include <memory>

MMU::MMU() {
    this->reset();
}

void MMU::link_devices(std::shared_ptr<PPU> ppu, std::shared_ptr<Joypad> joypad, std::shared_ptr<Timer> timer) {
    this->ppu = ppu;
    this->joypad = joypad;
    this->timer = timer;
}

void MMU::reset() {
    // Reset arrays to 0
    this->boot_rom.fill(0x00);
    this->vram.fill(0x00);
    this->ram.fill(0x00);
    this->oam.fill(0x00);
    this->io.fill(0x00);
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
    if (0x0000 <= addr && addr <= 0x7fff) {
        if (0x00 <= addr && addr <= 0xff && booting) {
            return this->boot_rom[addr];
        } else {
            return this->cartridge->read(addr);
        }
    }

    // VRAM
    if (0x8000 <= addr && addr <= 0x9fff) {
        return this->vram[addr - 0x8000];
    }

    // xRAM
    if (0xa000 <= addr && addr <= 0xbfff) {
        return this->cartridge->read(addr);
    }

    // WRAM
    if (0xc000 <= addr && addr <= 0xdfff) {
        return this->ram[addr - 0xc000];
    }

    // OAM
    if (0xfe00 <= addr && addr <= 0xfe9f) {
        return this->oam[addr - 0xfe00];
    }

    // I/O
    if (0xff00 <= addr && addr <= 0xff7f) {
        // Joypad
        if (addr == 0xff00) {
            return joypad->read(addr);
        }

        // Serial Data Transfer TODO: Implement Serial Data Transfer
        if (0xff01 <= addr && addr <= 0xff02) {
            // Supress to prevent console spam
            // std::cout << "Tried to read from Serial Data Transfer device. addr: " << (int)addr << std::endl;
            return 0;
        }

        // Timer
        if (0xff04 <= addr && addr <= 0xff07) {
            return this->timer->read(addr);
        }

        // Interrupt Flag
        if (addr == 0xff0f) {
            return this->interrupt_flag;
        }

        // Sound TODO: Implement sound
        if (0xff10 <= addr && addr <= 0xff26) {
            std::cout << "Tried to read from sound device. addr: " << (int)addr << std::endl;
            return 0;
        }

        // Waveform RAM TODO: Implement sound
        if (0xff30 <= addr && addr <= 0xff3f) {
            std::cout << "Tried to read Waveform RAM. addr: " << (int)addr << std::endl;
            return 0;
        }

        // LCD
        if (0xff40 <= addr && addr <= 0xff4b) {
            return this->ppu->read(addr);
        }
    }

    // HRAM
    if (0xff80 <= addr && addr <= 0xfffe) {
        return this->hram[addr - 0xff80];
    }

    // Interrupt Enable
    if (addr == 0xffff) {
        return this->interrupt_enable;
    }

    // Address did not match
    std::cout << "Tried to read unmapped memory address: " << (int)addr << std::endl;
    return 0;
}

void MMU::write(uint16_t addr, uint8_t data) {
    // Memory Bank Controller
    if (0x0000 <= addr && addr <= 0x7fff) {
        this->cartridge->write(addr, data);
        return;
    }

    // VRAM
    if (0x8000 <= addr && addr <= 0x9fff) {
        this->vram[addr - 0x8000] = data;
        return;
    }

    // xRAM
    if (0xa000 <= addr && addr <= 0xbfff) {
        this->cartridge->write(addr, data);
        return;
    }

    // WRAM
    if (0xc000 <= addr && addr <= 0xdfff) {
        this->ram[addr - 0xc000] = data;
        return;
    }

    // OAM
    if (0xfe00 <= addr && addr <= 0xfe9f) {
        this->oam[addr - 0xfe00] = data;
        return;
    }

    // IO
    if (0xff00 <= addr && addr <= 0xff7f) {
        // Joypad
        if (addr == 0xff00) {
            this->joypad->write(addr, data);
            return;
        }

        // Serial Data Transfer TODO: Implement Serial Data Transfer
        if (0xff01 <= addr && addr <= 0xff02) {
            // Supress to prevent console spam
            // std::cout << "Tried to write to Serial Data Transfer device. addr: " << (int)addr << " data: " << (int)data << std::endl;
            return;
        }

        // Timer
        if (0xff04 <= addr && addr <= 0xff07) {
            this->timer->write(addr, data);
            return;
        }

        // Interrupt Flag
        if (addr == 0xff0f) {
            this->interrupt_flag = data;
            return;
        }

        // Sound TODO: Implement sound
        if (0xff10 <= addr && addr <= 0xff26) {
            // Supress to prevent console spam
            // std::cout << "Tried to write to sound device. addr: " << (int)addr << " data: " << (int)data << std::endl;
            return;
        }

        // Waveform RAM TODO: Implement sound
        if (0xff30 <= addr && addr <= 0xff3f) {
            // Supress to prevent console spam
            // std::cout << "Tried to write to Waveform RAM. addr: " << (int)addr << " data: " << (int)data<< std::endl;
            return;
        }

        // LCD
        if (0xff40 <= addr && addr <= 0xff4b) {
            this->ppu->write(addr, data);
            return;
        }

        // Disable boot ROM
        if (addr == 0xff50) {
            this->disable_boot_rom(data);
            return;
        }

        std::cout << "Tried to write to unmapped memory address: " << (int)addr << " data: " << (int)data << std::endl;
    }

    // HRAM
    if (0xff80 <= addr && addr <= 0xfffe) {
        this->hram[addr - 0xff80] = data;
        return;
    }

    // Interrupt Enable
    if (addr == 0xffff) {
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
/*
void MMU::write_io(uint16_t addr, uint8_t data) {
    if (addr == IO_DISABLE_BOOT_ROM) {
        if (data != 0) this->disable_boot_rom();
    } else if (addr == IO_JOYPAD) {
        this->joypad->write(addr, data);
    } else if (addr == INTERRUPT_FLAG) {
        this->interrupt_flag = data;
    } else if (TIMER_DIVIDER <= addr && addr <= TIMER_CONTROL) {
        this->timer->write(addr, data);
    } else if (addr == DMA_TRANSFER) {
        std::cout << "DMA: data:" << (uint8_t)data;
        if (0x00 <= data && data <= 0xf1) {
            uint16_t start_addr = (data << 8);
            for (uint8_t i = 0; i <= 0x9f; i++) {
                this->write(0xfe00 + i, this->read(start_addr+i));
            }
        } else {
            std::cout << "Tried to use DMA transfer with invalid input: " << data << std::endl;
        }
    } else {
        this->io[addr - IO_START] = data;
    }
}*/
/*
uint8_t MMU::read_io(uint16_t addr) {
    if (addr == IO_JOYPAD) {
        this->joypad->read(addr);
    } else if (addr == INTERRUPT_FLAG) {
        return this->interrupt_flag & (0x1f);
    } else if (TIMER_START <= addr && addr <= TIMER_END) {
        this->timer->read(addr);
    } else {
        return this->io[addr - IO_START];
    }
    return 0;
}*/

void MMU::raise_interrupt_flag(uint8_t bitmask) {
    this->interrupt_flag |= bitmask;
}

void MMU::clear_interrupt_flag(uint8_t bitmask) {
    this->interrupt_flag &= ~bitmask;
}

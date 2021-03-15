//
// Created by Algot on 2021-03-11.
//

#include "MBC.h"
#include <iostream>

// ROM_Only_MBC
ROM_Only_MBC::ROM_Only_MBC(std::shared_ptr<std::vector<uint8_t>> rom) {
    this->rom = rom;
}

uint8_t ROM_Only_MBC::read(uint16_t addr) {
    return this->rom->at(addr);
}

void ROM_Only_MBC::write(uint16_t addr, uint8_t data) {
    std::cout << "Tried to write data: " << (int)data << " to addr: " << (int)addr << std::endl;
}

// MBC1
MBC1_MBC::MBC1_MBC(std::shared_ptr<std::vector<uint8_t>> rom, std::shared_ptr<std::vector<uint8_t>> ram) {
    this->rom = rom;
    this->ram = ram;

    this->ram_enable = 0;
    this->rom_bank_number = 1;
    this->ram_bank_number = 0;
    this->banking_mode = 0;
}

uint8_t MBC1_MBC::read(uint16_t addr) {
    if (0x0000 <= addr && addr <= 0x3fff) {
        // Simple ROM Banking Mode
        if ((this->banking_mode & (1 << 0)) == 0) {
            return this->rom->at(addr);
        }
        // RAM Banking Mode / Advanced ROM Banking Mode
        else if ((this->banking_mode & (1 << 0)) == 1) {
            uint16_t target_bank = (this->ram_bank_number << 5);
            target_bank &= this->rom_bank_mask((uint32_t)rom->size());
            return this->rom->at((addr) + (0x4000 * target_bank));
        }
    } else if (0x4000 <= addr && addr <= 0x7fff) {
        // Set target bank to 1 if it is 0
        uint16_t target_bank = this->rom_bank_number == 0 ? 1 : (this->rom_bank_number & 0x1f);
        target_bank |= (this->ram_bank_number << 5);
        target_bank &= this->rom_bank_mask((uint32_t)rom->size());
        return this->rom->at((addr - 0x4000) + (0x4000 * target_bank));

    } else if (0xa000 <= addr && addr <= 0xbfff) {
        if (this->ram_enable != 0xa) {
            std::cout << "Tried to access disabled xRAM. addr: " << (int)addr << std::endl;
            return 0xff;
        } else {
            uint8_t target_bank = 0;
            uint16_t target = addr - 0xa000;
            if (this->banking_mode == 1) {
                target_bank = this->ram_bank_number;
            }
            target_bank &= this->ram_bank_mask((uint32_t)ram->size());
            return this->ram->at((uint16_t)(target + (target_bank * 0x2000)));
        }
    }
    return 0;
}

void MBC1_MBC::write(uint16_t addr, uint8_t data) {
    if (0x0000 <= addr && addr <= 0x1fff) {
        this->ram_enable = data & 0b1111;
    } else if (0x2000 <= addr && addr <= 0x3fff) {
        this->rom_bank_number = data & 0b11111;
    } else if (0x4000 <= addr && addr <= 0x5fff) {
        this->ram_bank_number = data & 0b11;
    } else if (0x6000 <= addr && addr <= 0x7fff) {
        this->banking_mode = data & 0b1;
    } else if (0xa000 <= addr && addr <= 0xbfff) {
        if (this->ram_enable != 0xa) {
            std::cout << "Tried to write to disabled xRAM. addr: " << (int)addr << std::endl;
        } else {
            uint8_t target_bank = 0;
            uint16_t target = addr - 0xa000;
            if (this->banking_mode == 1) {
                target_bank = this->ram_bank_number;
            }
            target_bank &= this->ram_bank_mask((uint32_t)this->ram->size());
            this->ram->at((uint16_t)(target + (target_bank * 0x2000))) = data;
        }
    } else {
        std::cout << "Tried to write data: " << (int)data << " to addr: " << (int)addr << std::endl;
    }
}

uint16_t MBC1_MBC::rom_bank_mask(uint32_t size) {
    return (size >> 14) - 1;
}

uint16_t MBC1_MBC::ram_bank_mask(uint32_t size) {
    return (size >> 13) - 1;
}


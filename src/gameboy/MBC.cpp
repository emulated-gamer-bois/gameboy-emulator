//
// Created by Algot on 2021-03-11.
//

#include "MBC.h"
#include <iostream>

// MBC
uint16_t MBC::rom_bank_mask(uint32_t size) {
    return (size >> 14) - 1;
}

uint16_t MBC::ram_bank_mask(uint32_t size) {
    return (size >> 13) - 1;
}

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
            target_bank &= MBC::rom_bank_mask((uint32_t)rom->size());
            return this->rom->at((addr) + (0x4000 * target_bank));
        }
    } else if (0x4000 <= addr && addr <= 0x7fff) {
        // Set target bank to 1 if it is 0
        uint16_t target_bank = this->rom_bank_number == 0 ? 1 : (this->rom_bank_number & 0x1f);
        target_bank |= (this->ram_bank_number << 5);
        target_bank &= MBC::rom_bank_mask((uint32_t)rom->size());
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
            target_bank &= MBC::ram_bank_mask((uint32_t)ram->size());
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
            target_bank &= MBC::ram_bank_mask((uint32_t)this->ram->size());
            this->ram->at((uint16_t)(target + (target_bank * 0x2000))) = data;
        }
    } else {
        std::cout << "Tried to write data: " << (int)data << " to addr: " << (int)addr << std::endl;
    }
}

// MBC3
MBC3_MBC::MBC3_MBC(std::shared_ptr<std::vector<uint8_t>> rom, std::shared_ptr<std::vector<uint8_t>> ram) {
    this->rom = rom;
    this->ram = ram;

    this->rtc_register = 0;
    this->ram_timer_enable = 0;
    this->rom_bank_number = 1;
    this->ram_bank_number_rtc_register_select = 0;
    this->latch_clock_data = 0;

    this->rtc_halt = 0;
    this->rtc_subseconds = 0;
    this->rtc_seconds = 0;
    this->rtc_minutes = 0;
    this->rtc_hours = 0;
    this->rtc_days = 0;
    this->rtc_days_overflow = 0;

    this->rtc_halt_latched = 0;
    this->rtc_seconds_latched = 0;
    this->rtc_minutes_latched = 0;
    this->rtc_hours_latched = 0;
    this->rtc_days_latched = 0;
    this->rtc_days_overflow_latched = 0;
}

uint8_t MBC3_MBC::read(uint16_t addr) {
    if (0x0000 <= addr && addr <= 0x3fff) {
        return this->rom->at(addr);
    } else if (0x4000 <= addr && addr <= 0x7fff) {
        // Set target bank to 1 if it is 0
        uint16_t target_bank = this->rom_bank_number == 0 ? 1 : (this->rom_bank_number & 0x7f);

        target_bank &= MBC::rom_bank_mask((uint32_t)rom->size());
        return this->rom->at((addr - 0x4000) + (0x4000 * target_bank));
    } else if (0xa000 <= addr && addr <= 0xbfff) {
        if (this->ram_timer_enable != 0xa) {
            return 0xff;
        }
        if (this->ram_timer_enable == 0x0) {
            return 0xff;
        }
        uint16_t target;
        uint16_t target_bank;
        switch (this->ram_bank_number_rtc_register_select)
        {
            // Read RAM
            case 0x0:
            case 0x1:
            case 0x2:
            case 0x3:
                target_bank = this->ram_bank_number_rtc_register_select;
                target_bank &= MBC::ram_bank_mask((uint32_t)this->ram->size());
                target = (addr - 0xa000) + (0x2000 * target_bank);
                return this->ram->at(target);

            // Seconds
            case 0x8:
                return this->rtc_seconds_latched;

            // Minutes
            case 0x9:
                return this->rtc_minutes_latched;

            // Hours
            case 0xa:
                return this->rtc_hours_latched;

            // Lower 8 bits of Day Counter
            case 0xb:
                return (uint8_t)(this->rtc_days_latched & 0xff);

            // Upper 1 bit of Day Counter, Carry Bit, Halt Flag
            case 0xc:
                return (uint8_t) (((this->rtc_days_overflow_latched & (1 << 0)) << 7) |
                       ((this->rtc_halt_latched & (1 << 0)) << 6) |
                       (((this->rtc_days_latched & (1 << 8)) >> 8) << 0));

            default:
                return 0xff;
        }
    }
    return 0;
}

void MBC3_MBC::write(uint16_t addr, uint8_t data) {
    if (0x0000 <= addr && addr <= 0x1fff) {
        this->ram_timer_enable = data & 0b1111;
    } else if (0x2000 <= addr && addr <= 0x3fff) {
        this->rom_bank_number = data & 0b1111111;
    } else if (0x4000 <= addr && addr <= 0x5fff) {
        this->ram_bank_number_rtc_register_select = data;
    } else if (0x6000 <= addr && addr <= 0x7fff) {
        if (data == 1 && this->latch_clock_data == 0) {
            this->rtc_latch();
        }
        this->latch_clock_data = data;
    } else if (0xa000 <= addr && addr <= 0xbfff) {
        if (this->ram_timer_enable != 0xa) {
            return;
        }
        uint16_t target;
        switch (this->ram_bank_number_rtc_register_select)
        {
            // Read RAM
            case 0x0:
            case 0x1:
            case 0x2:
            case 0x3:
                target = (addr - 0xa000) + (0x2000 * this->ram_bank_number_rtc_register_select);
                this->ram->at(target) = data;
                break;

            // RTC Registers
            case 0x8:
                this->rtc_seconds = data & 0b111111;
                this->rtc_subseconds = 0;
                break;
            case 0x9:
                this->rtc_minutes = data & 0b111111;
                break;
            case 0xa:
                this->rtc_hours = data & 0b11111;
                break;
            case 0xb:
                this->rtc_days &= ~((uint16_t)0xff);
                this->rtc_days |= data & 0xff;
                break;
            case 0xc:
                this->rtc_days &= 0xff;
                this->rtc_days |= ((uint16_t)(data & 1)) << 8;

                this->rtc_halt = (data & (1 << 6)) >> 6;

                this->rtc_days_overflow = (data & (1 << 7)) >> 7;
                break;
            default:
                break;
        }
    } else {
        std::cout << "Tried to write data: " << (int)data << " to addr: " << (int)addr << std::endl;
    }
}

void MBC3_MBC::rtc_latch() {
    this->rtc_halt_latched = this->rtc_halt;
    this->rtc_seconds_latched = this->rtc_seconds;
    this->rtc_minutes_latched = this->rtc_minutes;
    this->rtc_hours_latched = this->rtc_hours;
    this->rtc_days_latched = this->rtc_days;
    this->rtc_days_overflow_latched = this->rtc_days_overflow;
}

// Update the timer with cycles @ 1,048,576Hz
void MBC3_MBC::update(uint8_t cycles) {
    if (this->rtc_halt) {
        return;
    }

    this->rtc_subseconds += cycles;

    if (this->rtc_subseconds >= 1048576) {
        this->rtc_subseconds -= 1048576;
        this->rtc_seconds += 1;
        this->rtc_seconds &= 0b111111;
    } else {
        return;
    }

    if (this->rtc_seconds == 60) {
        this->rtc_seconds -= 60;
        this->rtc_minutes += 1;
        this->rtc_minutes &= 0b111111;
    } else {
        return;
    }

    if (this->rtc_minutes == 60) {
        this->rtc_minutes -= 60;
        this->rtc_hours += 1;
        this->rtc_hours &= 0b11111;
    } else {
        return;
    }

    if (this->rtc_hours == 24) {
        this->rtc_hours -= 24;
        this->rtc_days += 1;
    } else {
        return;
    }

    if (this->rtc_days == 512) {
        this->rtc_hours -= 512;
        this->rtc_days_overflow = 1;
    }
}

//
// Created by Algot on 2021-03-11.
//

#include "MBC.h"
#include <iostream>

// MBC
uint16_t MBC::romBankMask(uint32_t size) {
    return (size >> 14) - 1;
}

uint16_t MBC::ramBankMask(uint32_t size) {
    return (size >> 13) - 1;
}

// ROM_Only_MBC
ROM_Only_MBC::ROM_Only_MBC(std::vector<uint8_t> *rom)
    : rom{rom} {
}

uint8_t ROM_Only_MBC::read(uint16_t addr) {
    if (0x0000 <= addr && addr <= 0x7fff) {
        return rom->at(addr);
    } else if (0xa000 <= addr && addr <= 0xbfff) {
        return 0xff;
    }
}

void ROM_Only_MBC::write(uint16_t addr, uint8_t data) {
    std::cout << "Tried to write data: " << (int)data << " to addr: " << (int)addr << std::endl;
}

// MBC1
MBC1_MBC::MBC1_MBC(std::vector<uint8_t> *rom, std::vector<uint8_t> *ram)
    : rom{rom}
    , ram{ram} {
    ramEnable = 0;
    romBankNumber = 1;
    ramBankNumber = 0;
    bankingMode = 0;
}

uint8_t MBC1_MBC::read(uint16_t addr) {
    if (0x0000 <= addr && addr <= 0x3fff) {
        // Simple ROM Banking Mode
        if ((bankingMode & (1 << 0)) == 0) {
            return rom->at(addr);
        }
        // RAM Banking Mode / Advanced ROM Banking Mode
        else if ((bankingMode & (1 << 0)) == 1) {
            uint16_t targetBank = (ramBankNumber << 5);
            targetBank &= MBC::romBankMask(static_cast<uint32_t>(rom->size()));
            return rom->at((addr) + (0x4000 * targetBank));
        }
    } else if (0x4000 <= addr && addr <= 0x7fff) {
        // Set target bank to 1 if it is 0
        uint16_t targetBank = romBankNumber == 0 ? 1 : (romBankNumber & 0x1f);
        targetBank |= (ramBankNumber << 5);
        targetBank &= MBC::romBankMask(static_cast<uint32_t>(rom->size()));
        return rom->at((addr - 0x4000) + (0x4000 * targetBank));

    } else if (0xa000 <= addr && addr <= 0xbfff) {
        if (ramEnable != 0xa) {
            std::cout << "Tried to access disabled xRAM. addr: " << (int)addr << std::endl;
            return 0xff;
        } else {
            uint8_t targetBank = 0;
            uint16_t target = addr - 0xa000;
            if (bankingMode == 1) {
                targetBank = ramBankNumber;
            }
            targetBank &= MBC::ramBankMask(static_cast<uint32_t>(ram->size()));
            return ram->at(target + static_cast<uint16_t>(targetBank * 0x2000));
        }
    }
    return 0;
}

void MBC1_MBC::write(uint16_t addr, uint8_t data) {
    if (0x0000 <= addr && addr <= 0x1fff) {
        ramEnable = data & 0b1111;
    } else if (0x2000 <= addr && addr <= 0x3fff) {
        romBankNumber = data & 0b11111;
    } else if (0x4000 <= addr && addr <= 0x5fff) {
        ramBankNumber = data & 0b11;
    } else if (0x6000 <= addr && addr <= 0x7fff) {
        bankingMode = data & 0b1;
    } else if (0xa000 <= addr && addr <= 0xbfff) {
        if (ramEnable != 0xa) {
            std::cout << "Tried to write to disabled xRAM. addr: " << (int)addr << std::endl;
        } else {
            uint8_t targetBank = 0;
            uint16_t target = addr - 0xa000;
            if (bankingMode == 1) {
                targetBank = ramBankNumber;
            }
            targetBank &= MBC::ramBankMask(static_cast<uint32_t>(ram->size()));
            ram->at(target + static_cast<uint16_t>(targetBank * 0x2000)) = data;
        }
    } else {
        std::cout << "Tried to write data: " << (int)data << " to addr: " << (int)addr << std::endl;
    }
}

// MBC3
MBC3_MBC::MBC3_MBC(std::vector<uint8_t> *rom, std::vector<uint8_t> *ram)
    : rom{rom}
    , ram{ram} {
    rtcRegister = 0;
    ramTimerEnable = 0;
    romBankNumber = 1;
    ramBankNumberRtcRegisterSelect = 0;
    latchClockData = 0;

    rtcHalt = 0;
    rtcSubseconds = 0;
    rtcSeconds = 0;
    rtcMinutes = 0;
    rtcHours = 0;
    rtcDays = 0;
    rtcDaysOverflow = 0;

    rtcHaltLatched = 0;
    rtcSecondsLatched = 0;
    rtcMinutesLatched = 0;
    rtcHoursLatched = 0;
    rtcDaysLatched = 0;
    rtcDaysOverflowLatched = 0;
}

uint8_t MBC3_MBC::read(uint16_t addr) {
    if (0x0000 <= addr && addr <= 0x3fff) {
        return rom->at(addr);
    } else if (0x4000 <= addr && addr <= 0x7fff) {
        // Set target bank to 1 if it is 0
        uint16_t targetBank = romBankNumber == 0 ? 1 : (romBankNumber & 0x7f);

        targetBank &= MBC::romBankMask(static_cast<uint32_t>(rom->size()));
        return rom->at((addr - 0x4000) + (0x4000 * targetBank));
    } else if (0xa000 <= addr && addr <= 0xbfff) {
        if (ramTimerEnable != 0xa) {
            return 0xff;
        }
        if (ramTimerEnable == 0x0) {
            return 0xff;
        }
        uint16_t target;
        uint16_t targetBank;
        switch (ramBankNumberRtcRegisterSelect)
        {
            // Read RAM
            case 0x0:
            case 0x1:
            case 0x2:
            case 0x3:
                targetBank = ramBankNumberRtcRegisterSelect;
                targetBank &= MBC::ramBankMask(static_cast<uint32_t>(ram->size()));
                target = (addr - 0xa000) + (0x2000 * targetBank);
                return ram->at(target);

            // Seconds
            case 0x8:
                return rtcSecondsLatched;

            // Minutes
            case 0x9:
                return rtcMinutesLatched;

            // Hours
            case 0xa:
                return rtcHoursLatched;

            // Lower 8 bits of Day Counter
            case 0xb:
                return static_cast<uint8_t>(rtcDaysLatched & 0xff);

            // Upper 1 bit of Day Counter, Carry Bit, Halt Flag
            case 0xc:
                return static_cast<uint8_t>(((rtcDaysOverflowLatched & (1 << 0)) << 7) |
                       ((rtcHaltLatched & (1 << 0)) << 6) |
                       (((rtcDaysLatched & (1 << 8)) >> 8) << 0));

            default:
                return 0xff;
        }
    }
    return 0;
}

void MBC3_MBC::write(uint16_t addr, uint8_t data) {
    if (0x0000 <= addr && addr <= 0x1fff) {
        ramTimerEnable = data & 0b1111;
    } else if (0x2000 <= addr && addr <= 0x3fff) {
        romBankNumber = data & 0b1111111;
    } else if (0x4000 <= addr && addr <= 0x5fff) {
        ramBankNumberRtcRegisterSelect = data;
    } else if (0x6000 <= addr && addr <= 0x7fff) {
        if (data == 1 && latchClockData == 0) {
            rtcLatch();
        }
        latchClockData = data;
    } else if (0xa000 <= addr && addr <= 0xbfff) {
        if (ramTimerEnable != 0xa) {
            return;
        }
        uint16_t target;
        switch (ramBankNumberRtcRegisterSelect)
        {
            // Read RAM
            case 0x0:
            case 0x1:
            case 0x2:
            case 0x3:
                target = (addr - 0xa000) + (0x2000 * ramBankNumberRtcRegisterSelect);
                ram->at(target) = data;
                break;

            // RTC Registers
            case 0x8:
                rtcSeconds = data & 0b111111;
                rtcSubseconds = 0;
                break;
            case 0x9:
                rtcMinutes = data & 0b111111;
                break;
            case 0xa:
                rtcHours = data & 0b11111;
                break;
            case 0xb:
                rtcDays &= ~(0x00ff);
                rtcDays |= data & 0xff;
                break;
            case 0xc:
                rtcDays &= 0xff;
                rtcDays |= (static_cast<uint16_t>(data & 1)) << 8;

                rtcHalt = (data & (1 << 6)) >> 6;

                rtcDaysOverflow = (data & (1 << 7)) >> 7;
                break;
            default:
                break;
        }
    } else {
        std::cout << "Tried to write data: " << (int)data << " to addr: " << (int)addr << std::endl;
    }
}

void MBC3_MBC::rtcLatch() {
    rtcHaltLatched = rtcHalt;
    rtcSecondsLatched = rtcSeconds;
    rtcMinutesLatched = rtcMinutes;
    rtcHoursLatched = rtcHours;
    rtcDaysLatched = rtcDays;
    rtcDaysOverflowLatched = rtcDaysOverflow;
}

// Update the timer with cycles @ 1,048,576Hz
void MBC3_MBC::update(uint8_t cycles) {
    if (rtcHalt) {
        return;
    }

    rtcSubseconds += cycles;

    if (rtcSubseconds >= 1048576) {
        rtcSubseconds -= 1048576;
        rtcSeconds += 1;
        rtcSeconds &= 0b111111;
    } else {
        return;
    }

    if (rtcSeconds == 60) {
        rtcSeconds -= 60;
        rtcMinutes += 1;
        rtcMinutes &= 0b111111;
    } else {
        return;
    }

    if (rtcMinutes == 60) {
        rtcMinutes -= 60;
        rtcHours += 1;
        rtcHours &= 0b11111;
    } else {
        return;
    }

    if (rtcHours == 24) {
        rtcHours -= 24;
        rtcDays += 1;
    } else {
        return;
    }

    if (rtcDays == 512) {
        rtcHours -= 512;
        rtcDaysOverflow = 1;
    }
}

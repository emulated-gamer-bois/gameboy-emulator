//
// Created by Algot on 2021-03-11.
//

#include "Joypad.h"
#include "MMU/MMU.h"
#include <iostream>
#include <utility>
#include "Definitions.h"
Joypad::Joypad(std::shared_ptr<MMU> mmu):mmu(std::move(mmu)) {}


void Joypad::reset() {
    // Select direction buttons by default
    this->joypadSelect = JOYPAD_SEL_DIRECTIONS;
    this->joypad = 0xff;
}

uint8_t Joypad::read(uint16_t addr) const {
    switch (addr) {
        case JOYPAD:
            if ((this->joypadSelect & JOYPAD_SEL_DIRECTIONS)) {
                return ((this->joypad >> 0) & 0xf);
            } else {
                return ((this->joypad >> 4) & 0xf);
            }
        default:
            std::cout << "Tried to read address: " << (int)addr << " on Joypad." << std::endl;
            return 0;
    }
}

void Joypad::write(uint16_t addr, uint8_t data) {
    switch (addr) {
        case JOYPAD:
            this->joypadSelect = data;
            break;
        default:
            std::cout << "Tried to write data: " << (int)data << " to address: " << (int)addr << " on Joypad." << std::endl;
            break;
    }
}

void Joypad::release(uint8_t button) {
    this->joypad |= (1 << button);
}

void Joypad::press(uint8_t button) {
    this->joypad &= ~(1 << button);

    // Raise interrupt flag
    this->mmu->raiseInterruptFlag(CONTROLLER_IF_BIT);
}


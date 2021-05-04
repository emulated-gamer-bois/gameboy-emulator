
#include "Joypad.h"

Joypad::Joypad(std::shared_ptr<MMU> mmu):mmu(std::move(mmu)) {}


void Joypad::reset() {
    // Select direction buttons by default
    joypadSelect = JOYPAD_SEL_DIRECTIONS;
    joypad = 0xff;
}

uint8_t Joypad::read(uint16_t addr) const {
    switch (addr) {
        case JOYPAD:
            if ((joypadSelect & JOYPAD_SEL_DIRECTIONS)) {
                return ((joypad >> 0) & 0xf);
            } else {
                return ((joypad >> 4) & 0xf);
            }
        default:
            std::cout << "Tried to read address: " << (int)addr << " on Joypad." << std::endl;
            return 0;
    }
}

void Joypad::write(uint16_t addr, uint8_t data) {
    switch (addr) {
        case JOYPAD:
            joypadSelect = data;
            break;
        default:
            std::cout << "Tried to write data: " << (int)data << " to address: " << (int)addr << " on Joypad." << std::endl;
            break;
    }
}

void Joypad::release(uint8_t button) {
    joypad |= (1 << button);
}

void Joypad::press(uint8_t button) {
    joypad &= ~(1 << button);

    // Raise interrupt flag
    mmu->raiseInterruptFlag(CONTROLLER_IF_BIT);
}


//
// Created by Algot on 2021-03-11.
//

#include "Timer.h"
#include "MMU.h"
#include <iostream>

Timer::Timer(std::shared_ptr<MMU> mmu)
    : mmu{mmu} {
    reset();
}

void Timer::reset() {
    divider = 0;
    counter = 0;
    modulo = 0;
    control = 0;
}

void Timer::update(uint16_t cycles) {
    // Always increase divider
    uint16_t oldDivider = divider;
    divider += (cycles*4);

    // Timer activated
    if (control & (1 << 0)) {
        // Timer speed
        uint8_t speed = control & 0b11;
        const uint16_t speedMask[] = {0xfc00, 0xfff0, 0xffc0, 0xff00};
        const uint8_t speedOffs[] = {10, 4, 6, 8};

        uint16_t mask = speedMask[speed];
        uint8_t offs = speedOffs[speed];

        // Check for counter overflow
        // Increase counter
        uint8_t increaseCounter = (((divider & mask) - (oldDivider & mask)) >> offs);
        if ((uint16_t)(counter + increaseCounter) > 0xff) {
            // Add modulo
            counter += (modulo + increaseCounter);
            // Raise interrupt request for Timer interrupt
            mmu->raise_interrupt_flag(1 << 2);
        } else {
            counter += increaseCounter;
        }
    }
}

uint8_t Timer::read(uint16_t addr) const {
    switch (addr) {
        case TIMER_DIVIDER:
            return (uint8_t)(divider >> 8);
        case TIMER_COUNTER:
            return counter;
        case TIMER_MODULO:
            return modulo;
        case TIMER_CONTROL:
            return control & 0b111;
        default:
            std::cout << "Tried to read address: " << (int)addr << " on Timer." << std::endl;
            return 0;
    }
}

void Timer::write(uint16_t addr, uint8_t data) {
    switch (addr) {
        case TIMER_DIVIDER:
            divider = 0;
            break;
        case TIMER_COUNTER:
            counter = data;
            break;
        case TIMER_MODULO:
            modulo = data;
            break;
        case TIMER_CONTROL:
            control = data & 0b111;
            break;
        default:
            std::cout << "Tried to write data: " << (int)data << " to address: " << (int)addr << " on Timer." << std::endl;
    }
}

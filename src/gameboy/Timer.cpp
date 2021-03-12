//
// Created by Algot on 2021-03-11.
//

#include "Timer.h"
#include "MMU.h"

Timer::Timer(std::shared_ptr<MMU> mmu) {
    this->mmu = mmu;

    this->divider = 0;
    this->counter = 0;
    this->modulo = 0;
    this->control = 0;
}

void Timer::update(uint16_t cycles) {
    // Timer activated
    if (this->control & (1 << 0)) {
        // Increase divider
        uint16_t old_divider = this->divider;
        this->divider += (cycles*4);

        // Timer speed
        uint8_t speed = this->control & 0b11;
        const uint16_t speed_mask[] = {0xfc00, 0xfff0, 0xffc0, 0xff00};
        const uint8_t speed_offs[] = {10, 4, 6, 8};

        uint16_t mask = speed_mask[speed];
        uint8_t offs = speed_offs[speed];

        // Check for counter overflow
        // Increase counter
        uint8_t increase_counter = (((this->divider & mask) - (old_divider & mask)) >> offs);
        if ((uint16_t)(this->counter + increase_counter) > 0xff) {
            // Add modulo
            this->counter += (this->modulo + increase_counter);
            // Raise interrupt request for Timer interrupt
            this->mmu->raise_interrupt_flag(1 << 2);
        } else {
            this->counter += increase_counter;
        }
    }
}

uint8_t Timer::read(uint16_t addr) const {
    switch (addr) {
        case TIMER_DIVIDER:
            return (uint8_t)(this->divider >> 8);
        case TIMER_COUNTER:
            return this->counter;
        case TIMER_MODULO:
            return this->modulo;
        case TIMER_CONTROL:
            return this->control & 0b111;
        default:
            std::cout << "Tried to read address: " << (int)addr << " on Timer." << std::endl;
            return 0;
    }
}

void Timer::write(uint16_t addr, uint8_t data) {
    switch (addr) {
        case TIMER_DIVIDER:
            this->divider = 0;
            break;
        case TIMER_COUNTER:
            this->counter = data;
            break;
        case TIMER_MODULO:
            this->modulo = data;
            break;
        case TIMER_CONTROL:
            this->control = data & 0b111;
            break;
        default:
            std::cout << "Tried to write data: " << (int)data << " to address: " << (int)addr << " on Timer." << std::endl;
    }
}

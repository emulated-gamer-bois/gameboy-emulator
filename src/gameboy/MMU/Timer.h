//
// Created by Algot on 2021-03-11.
//

#pragma once

#include <cstdint>
#include <memory>

#define TIMER_DIVIDER       0xff04
#define TIMER_COUNTER       0xff05
#define TIMER_MODULO        0xff06
#define TIMER_CONTROL       0xff07

class MMU;

class Timer {
public:
    explicit Timer(std::shared_ptr<MMU> mmu);

    // Reset private registers
    void reset();

    uint8_t read(uint16_t addr) const;
    void write(uint16_t addr, uint8_t data);

    // Update the timer with cycles @ 1,048,576Hz
    void update(uint16_t cycles);

private:
    // MMU used to set interrupt flags
    std::shared_ptr<MMU> mmu;

    // Private registers
    uint16_t divider;
    uint8_t counter;
    uint8_t modulo;
    uint8_t control;
};

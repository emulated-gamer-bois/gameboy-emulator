#pragma once

#include <cstdint>
#include <memory> //ptr
#define TIMER_DIVIDER       0xff04
#define TIMER_COUNTER       0xff05
#define TIMER_MODULO        0xff06
#define TIMER_CONTROL       0xff07

class MMU;

class Timer {
public:
    explicit Timer(std::shared_ptr<MMU> mmu);

    /**
     * Reset the divider-, counter-, modulo- and control-register.
     */
    void reset();

    /**
     * Return 1 byte according to the specified address.
     * If the address is not mapped to any of the four registers return 0.
     * @param addr memory address
     */
    uint8_t read(uint16_t addr) const;

    /**
     * Write to register mapped to address.
     * If address correspond to the divider register, write 0.
     * Else write data to the corresponding register.
     */
    void write(uint16_t addr, uint8_t data);

    /**
     * Update the counter registers according to the number of cycles.
     * Behaviour depends on control register. A detailed description is available on: https://gbdev.io/pandocs/#timer-and-divider-registers.
     * @param cycles number oif cycles @ 1,048,576Hz to update the timer
     */
    void update(uint16_t cycles);

private:
    // MMU used to set interrupt flags
    std::shared_ptr<MMU> mmu;

    // Address mapped registers
    uint16_t divider{};
    uint8_t counter{};
    uint8_t modulo{};
    uint8_t control{};
};


#pragma once

#include <cstdint>
#include <memory> //ptr

#define JOYPAD                  0xff00
#define JOYPAD_SEL_BUTTONS      0x10
#define JOYPAD_SEL_DIRECTIONS   0x20
#define JOYPAD_RIGHT            0
#define JOYPAD_LEFT             1
#define JOYPAD_UP               2
#define JOYPAD_DOWN             3
#define JOYPAD_A                4
#define JOYPAD_B                5
#define JOYPAD_SELECT           6
#define JOYPAD_START            7

// Forward declaration
class MMU;
/**
 * This class provides the functionality of the Game Boy Joypad through reading and writing
 * to the specific addresses regarding this.
 *
 * */
class Joypad {

public:
    explicit Joypad(std::shared_ptr<MMU> mmu);
    /**
     * Resets the Joypad to its initial state.
     * */
    void reset();
    /**
     * Reads the content of the Joypad.
     * @param addr address to be read from, if not equal to JOYPAD(0xFF00), 0 is returned.
     * @return returns status of Joypad.
     *
     * */
    uint8_t read(uint16_t addr) const;
    /**
     * If the correct address is given (0xFF00), the data provided is written to the Joypad.
     * @param addr address of Joypad.
     * @param data data to write to the given address.
     *
     * */
    void write(uint16_t addr, uint8_t data);
    /**
     * Upon the release of a button, this bit of the joypad is reset to
     * 1.
     * @param button to reset.
     * */
    void release(uint8_t button);
    /**
     * Upon the press of a button, this bit of the joypad is set to
     * 0.
     * @param button to set.
     * */
    void press(uint8_t button);

private:
    // MMU used to set interrupt flags
    std::shared_ptr<MMU> mmu;

    // Private registers
    uint8_t joypadSelect{JOYPAD_SEL_DIRECTIONS};
    uint8_t joypad{0xFF};
};

//
// Created by Algot on 2021-03-11.
//

#pragma once

#include <cstdint>
#include <memory>
#include <iostream>

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

class Joypad {
private:
    std::shared_ptr<MMU> mmu;
    uint8_t joypad_select;
    uint8_t joypad;
public:
    Joypad(std::shared_ptr<MMU> mmu);
    void release(uint8_t button);
    void press(uint8_t button);
    uint8_t read(uint16_t addr) const;
    void write(uint16_t addr, uint8_t data);
};

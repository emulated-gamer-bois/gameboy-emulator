#pragma once

#include <cstdint>
/**
 * This union is used to represent pairs of 8-bit registers making them usable as
 * both two single 8-bit registers as well as one combined 16-bit register.
 * */
union RegisterPair {
    struct {
        uint8_t low_8;
        uint8_t high_8;
    };
    uint16_t all_16;
};

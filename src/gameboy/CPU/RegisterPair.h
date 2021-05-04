
#pragma once

#include <cstdint>

union RegisterPair {
    struct {
        uint8_t low_8;
        uint8_t high_8;
    };
    uint16_t all_16;
};

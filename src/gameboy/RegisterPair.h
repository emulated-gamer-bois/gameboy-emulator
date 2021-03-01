//
// Created by davidm on 2021-02-14.
//

#pragma once

#include <cstdint>

union RegisterPair {
    struct {
        uint8_t low_8;
        uint8_t high_8;
    };
    uint16_t all_16;
};

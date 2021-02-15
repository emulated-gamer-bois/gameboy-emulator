//
// Created by davidm on 2021-02-14.
//

#ifndef LAME_BOY_REGISTER_PAIR_H
#define LAME_BOY_REGISTER_PAIR_H

union RegisterPair {
    struct {
        uint8_t low_8;
        uint8_t high_8;
    };
    uint16_t all_16;
};

#endif //LAME_BOY_REGISTER_PAIR_H

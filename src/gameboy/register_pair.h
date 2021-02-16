//
// Created by davidm on 2021-02-14.
//

#ifndef LAME_BOY_REGISTER_PAIR_H
#define LAME_BOY_REGISTER_PAIR_H

union RegisterPair {
    struct {
        u_int8_t low_8;
        u_int8_t high_8;
    };
    u_int16_t all_16;
};

#endif //LAME_BOY_REGISTER_PAIR_H

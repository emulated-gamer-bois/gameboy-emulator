//
// Created by isaaklindgren on 2021-02-23.
//

#ifndef LAME_BOY_FLAGS_H
#define LAME_BOY_FLAGS_H
union Flags {
    struct {
    /**
     * Needs to be in this order
     * */
        unsigned int unused_0 : 1;
        unsigned int unused_1 : 1;
        unsigned int unused_2 : 1;
        unsigned int unused_3 : 1;

        unsigned int c : 1;
        unsigned int h : 1;
        unsigned int n : 1;
        unsigned int z : 1;
    };
    uint8_t all_8;
};
#endif //LAME_BOY_FLAGS_H

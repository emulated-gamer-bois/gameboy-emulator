//
// Created by davidm on 2021-02-14.
//

#include "gtest/gtest.h"
#include "../src/gameboy/register_pair.h"

TEST(REGISTER_PAIR, READ_WRITE) {
    RegisterPair reg;
    reg.all_16 = 0xFFAA;
    ASSERT_EQ(reg.low_8, 0xAA);
    ASSERT_EQ(reg.high_8, 0xFF);

    reg.low_8 = 0xBB;
    reg.high_8 = 0x55;
    ASSERT_EQ(reg.all_16, 0x55BB);
}
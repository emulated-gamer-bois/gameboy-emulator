//
// Created by davidm on 2021-02-14.
//

#include "gtest/gtest.h"
#include "../src/gameboy/register_pair.h"
#include "../src/gameboy/Flags.h"

TEST(REGISTER_PAIR, READ_WRITE) {
    RegisterPair reg;
    reg.all_16 = 0xFFAA;
    ASSERT_EQ(reg.low_8, 0xAA);
    ASSERT_EQ(reg.high_8, 0xFF);

    reg.low_8 = 0xBB;
    reg.high_8 = 0x55;
    ASSERT_EQ(reg.all_16, 0x55BB);
}

TEST(FLAGS,FLAGS){
    Flags f;
    f.all_8 =0;
    ASSERT_TRUE(f.z ==0);
    f.z=1;
    ASSERT_EQ(f.all_8 , 0x80);
    f.all_8=0;
    f.c = 1;
    ASSERT_EQ(f.all_8, 0x10);
    f.all_8=0;
    f.h = 1;
    ASSERT_EQ(f.all_8, 0x20);
    f.all_8=0;
    f.n = 1;
    ASSERT_EQ(f.all_8, 0x40);
}
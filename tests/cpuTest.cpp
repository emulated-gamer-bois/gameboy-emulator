//
// Created by davidm on 2021-02-12.
//

#include <cpu.h>
#include "gtest/gtest.h"

TEST(CPU, TestRegier){
    CPU cpu;
    ASSERT_EQ(cpu.one() ,1);
}

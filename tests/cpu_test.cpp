//
// Created by davidm on 2021-02-12.
//

#include <memory>

#include "gtest/gtest.h"
#include "../src/gameboy/cpu.h"

TEST(CPU, Execute_NOP_Instruction){
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<CPU> cpu(new CPU(0x00, mmu));

    mmu->write(0, 0);
    cpu->execute_cycle();

    ASSERT_EQ(mmu->read(0), 0);
    ASSERT_EQ(cpu->getPC(), 0x01);
}

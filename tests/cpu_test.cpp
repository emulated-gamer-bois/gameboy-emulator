//
// Created by davidm on 2021-02-12.
//

#include <memory>

#include "gtest/gtest.h"
#include "../src/gameboy/cpu.h"

TEST(CPU, Execute_NOP_Instruction){
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<CPU> cpu(new CPU(0x00, mmu));

    mmu->write_ONLY_IN_TESTS(0x00, 0x00);
    cpu->execute_cycle();

    ASSERT_EQ(mmu->read(0x00), 0x00);
    ASSERT_EQ(cpu->getPC(), 0x01);
}

TEST(CPU, Execute_LD_SP_D16_Instruction) {
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<CPU> cpu(new CPU(0x00, mmu));
    mmu->disable_boot_rom();

    mmu->write_ONLY_IN_TESTS(0x00, 0x31);
    mmu->write_ONLY_IN_TESTS(0x01, 0xFF);
    mmu->write_ONLY_IN_TESTS(0x02, 0xAA);
    cpu->execute_cycle();

    ASSERT_EQ(cpu->getPC(), 0x03);
    ASSERT_EQ(cpu->getSP(), 0xAAFF);
}
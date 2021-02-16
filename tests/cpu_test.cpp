//
// Created by davidm on 2021-02-12.
//

#include <memory>

#include "gtest/gtest.h"
#include "../src/gameboy/cpu.h"

TEST(CPU, Execute_NOP_Instruction) {
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<CPU> cpu(new CPU(0x00, mmu));

    mmu->write(0x00, 0x00);
    cpu->execute_cycle();

    ASSERT_EQ(mmu->read(0x00), 0x00);
    ASSERT_EQ(cpu->getPC(), 0x01);
}

TEST(CPU, Execute_LD_SP_D16_Instruction) {
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<CPU> cpu(new CPU(0x00, mmu));

    mmu->write(0x00, 0x31);
    mmu->write(0x01, 0xFF);
    mmu->write(0x02, 0xAA);
    cpu->execute_cycle();

    ASSERT_EQ(cpu->getPC(), 0x03);
    ASSERT_EQ(cpu->getSP(), 0xAAFF);
}

TEST(CPU, SET_FLAGS) {
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<CPU> cpu(new CPU(0x00, mmu));

    cpu->setZNFlags(0x00, false);
    ASSERT_EQ(cpu->getFlags() & 0x80, 0x80);
    ASSERT_EQ(cpu->getFlags() & 0x40, 0x00);

    cpu->setZNFlags(0x01, true);
    ASSERT_EQ(cpu->getFlags() & 0x80, 0x00);
    ASSERT_EQ(cpu->getFlags() & 0x40, 0x40);

    cpu->setHFlag(0x0F, 0x01);
    ASSERT_EQ(cpu->getFlags() & 0x20, 0x20);
    cpu->setHFlag(0x0D, 0x01);
    ASSERT_EQ(cpu->getFlags() & 0x20, 0x00);


    cpu->setCFlag(0xFF, 0x01);
    ASSERT_EQ(cpu->getFlags() & 0x10, 0x10);

}

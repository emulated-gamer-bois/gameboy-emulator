//
// Created by davidm on 2021-02-12.
//

#include <memory>

#include "gtest/gtest.h"
#include "../src/gameboy/cpu.h"
#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test


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

TEST(CPU, FUNDAMENTAL_FUNCTIONS) {
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

    cpu->orA(0x55);
    ASSERT_EQ(cpu->getA(), 0x55);

    cpu->xorA(0x04);
    ASSERT_EQ(cpu->getA(), 0x51);

    cpu->andA(0x4F);
    ASSERT_EQ(cpu->getA(), 0x41);

    mmu->write(cpu->getPC(), 0x07); //RLCA
    mmu->write(cpu->getPC()+1, 0x07);
    cpu->execute_cycle();
    ASSERT_EQ(cpu->getA(), 0x82);
    cpu->execute_cycle();
    ASSERT_EQ(cpu->getA(), 0x05);
    ASSERT_EQ(cpu->getFlags() & 0x10, 0x10);

    mmu->write(cpu->getPC(), 0x17); //RLA
    cpu->execute_cycle();
    ASSERT_EQ(cpu->getA(), 0x0B);
    ASSERT_EQ(cpu->getFlags() & 0x10, 0x00);

    mmu->write(cpu->getPC(), 0x0F); //RRCA
    mmu->write(cpu->getPC()+1, 0x0F);
    cpu->execute_cycle();
    ASSERT_EQ(cpu->getA(), 0x85);
    ASSERT_EQ(cpu->getFlags() & 0x10, 0x10);

    mmu->write(cpu->getPC(), 0x1F); //RRA
    cpu->execute_cycle();
    ASSERT_EQ(cpu->getA(), 0xC2);
    ASSERT_EQ(cpu->getFlags() & 0x10, 0x10);

}



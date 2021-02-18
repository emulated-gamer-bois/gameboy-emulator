//
// Created by davidm on 2021-02-12.
//

#include <memory>
#include "gtest/gtest.h"
#include "../src/gameboy/cpu.h"

/*TEST(CPU, Execute_NOP_Instruction) {
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<CPU> cpu(new CPU(0x00, mmu));

    mmu->write(0x00, 0x00);
    cpu->execute_cycle();

    ASSERT_EQ(mmu->read(0x00), 0x00);
    ASSERT_EQ(cpu->PC, 0x01);
}

TEST(CPU, Execute_LD_SP_D16_Instruction) {
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<CPU> cpu(new CPU(0x00, mmu));

    mmu->write(0x00, 0x31);
    mmu->write(0x01, 0xFF);
    mmu->write(0x02, 0xAA);
    cpu->execute_cycle();

    ASSERT_EQ(cpu->PC, 0x03);
    ASSERT_EQ(cpu->SP, 0xAAFF);
}*/

TEST(CPU, FUNDAMENTAL_FUNCTIONS) {
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<CPU> cpu(new CPU(0x00, mmu));

    cpu->setZNFlags(0x00, false);
    ASSERT_EQ(cpu->AF.low_8 & 0x80, 0x80);
    ASSERT_EQ(cpu->AF.low_8 & 0x40, 0x00);

    cpu->setZNFlags(0x01, true);
    ASSERT_EQ(cpu->AF.low_8 & 0x80, 0x00);
    ASSERT_EQ(cpu->AF.low_8 & 0x40, 0x40);

    cpu->setHFlag(0x0F, 0x01);
    ASSERT_EQ(cpu->AF.low_8 & 0x20, 0x20);
    cpu->setHFlag(0x0D, 0x01);
    ASSERT_EQ(cpu->AF.low_8 & 0x20, 0x00);

    cpu->setCFlag(0xFF, 0x01);
    ASSERT_EQ(cpu->AF.low_8 & 0x10, 0x10);
    
    cpu->orA(0x55);
    ASSERT_EQ(cpu->AF.high_8, 0x55);

    cpu->xorA(0x04);
    ASSERT_EQ(cpu->AF.high_8, 0x51);

    cpu->andA(0x4F);
    ASSERT_EQ(cpu->AF.high_8, 0x41);

    cpu->rlc(cpu->AF.high_8);
    ASSERT_EQ(cpu->AF.high_8, 0x82);
    ASSERT_EQ(cpu->AF.low_8 & 0x10, 0x00);

    cpu->rlc(cpu->AF.high_8);
    ASSERT_EQ(cpu->AF.high_8, 0x05);
    ASSERT_EQ(cpu->AF.low_8 & 0x10, 0x10);

    cpu->rl(cpu->AF.high_8);
    ASSERT_EQ(cpu->AF.high_8, 0x0B);
    ASSERT_EQ(cpu->AF.low_8 & 0x10, 0x00);

    cpu->rrc(cpu->AF.high_8);
    ASSERT_EQ(cpu->AF.high_8, 0x85);
    ASSERT_EQ(cpu->AF.low_8 & 0x10, 0x10);

    cpu->rr(cpu->AF.high_8);
    ASSERT_EQ(cpu->AF.high_8, 0xC2);
    ASSERT_EQ(cpu->AF.low_8 & 0x10, 0x10);

    cpu->setA(0x05);
    cpu->setB(0x0E);
    uint8_t val = cpu->AF.high_8 + cpu->BC.high_8;
    cpu->addA(cpu->BC.high_8, false);
    ASSERT_EQ(cpu->AF.high_8, val);
    ASSERT_EQ(cpu->AF.low_8 & 0xF0 , 0x20); // Only H-flag set.

    cpu->setA(8);
    cpu->setB(8);
    cpu->subA(cpu->BC.high_8, false);
    ASSERT_EQ(0x00, cpu->AF.high_8);
    ASSERT_EQ(cpu->AF.low_8 & 0xF0, 0xF0); // All flags set

    cpu->setA(255);
    cpu->setB(1);
    val = cpu->AF.high_8 + cpu->BC.high_8;
    cpu->addA(cpu->BC.high_8, false);
    ASSERT_EQ(cpu->AF.high_8, val);
    ASSERT_EQ(cpu->AF.low_8, 0xB0); //Z,H and C.

    cpu->setA(15);
    cpu->setB(5);
    val = cpu->AF.high_8 + cpu->BC.high_8;
    cpu->addA(cpu->BC.high_8, true);
    ASSERT_EQ(cpu->AF.high_8, val + 1);
    ASSERT_EQ(cpu->AF.low_8, 0x20); // Only H flag.

    //setting C-flag for next test.
    cpu->AF.low_8 |= 0x10;

    cpu->setA(10);
    cpu->setB(5);
    val = cpu->AF.high_8 - cpu->BC.high_8;
    cpu->subA(cpu->BC.high_8, true);
    ASSERT_EQ(cpu->AF.high_8, val - 1);
    ASSERT_EQ(cpu->AF.low_8 & 0xF0, 0x70); //H, C, N set

    int8_t prevB = cpu->BC.high_8;
    cpu->increment8(cpu->BC.high_8);
    ASSERT_EQ(cpu->BC.high_8, prevB + 1);

    prevB = cpu->BC.high_8;
    cpu->decrement8(cpu->BC.high_8);
    ASSERT_EQ(cpu->BC.high_8, prevB - 1);
    ASSERT_EQ(cpu->AF.low_8 & 0x40, 0x40);

    uint16_t prevSP = cpu->SP.all_16;
    cpu->increment16(cpu->SP.all_16);
    ASSERT_EQ(cpu->SP.all_16, prevSP + 1);

    prevSP = cpu->SP.all_16;
    cpu->decrement16(cpu->SP.all_16);
    ASSERT_EQ(cpu->SP.all_16, prevSP - 1);
}
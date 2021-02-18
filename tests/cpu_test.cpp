//
// Created by davidm on 2021-02-12.
//

#include <memory>
#include <bitset>
#include "gtest/gtest.h"
#include "../src/gameboy/cpu.h"

#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test


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

    mmu->write(cpu->PC, 0x07); //RLCA
    mmu->write(cpu->PC + 1, 0x07);
    cpu->execute_cycle();
    ASSERT_EQ(cpu->AF.high_8, 0x82);
    cpu->execute_cycle();
    ASSERT_EQ(cpu->AF.high_8, 0x05);
    ASSERT_EQ(cpu->AF.low_8 & 0x10, 0x10);

    mmu->write(cpu->PC, 0x17); //RLA
    cpu->execute_cycle();
    ASSERT_EQ(cpu->AF.high_8, 0x0B);
    ASSERT_EQ(cpu->AF.low_8 & 0x10, 0x00);

    mmu->write(cpu->PC, 0x0F); //RRCA
    mmu->write(cpu->PC + 1, 0x0F);
    cpu->execute_cycle();
    ASSERT_EQ(cpu->AF.high_8, 0x85);
    ASSERT_EQ(cpu->AF.low_8 & 0x10, 0x10);

    mmu->write(cpu->PC, 0x1F); //RRA
    cpu->execute_cycle();
    ASSERT_EQ(cpu->AF.high_8, 0xC2);
    ASSERT_EQ(cpu->AF.low_8 & 0x10, 0x10);


    mmu->write(cpu->PC, 0x80); //ADD B
    cpu->setA(5);
    cpu->setB(14);
    uint8_t val = cpu->AF.high_8 + cpu->BC.high_8;
    cpu->execute_cycle();
    ASSERT_EQ(cpu->AF.high_8, val);
    ASSERT_EQ(cpu->AF.low_8, 0x20); // Only H-flag set.


    mmu->write(cpu->PC, 0x90);//SUB B
    cpu->setA(3);
    cpu->setB(5);
    val = cpu->AF.high_8 - cpu->BC.high_8;
    cpu->execute_cycle();
    std::string binVal = std::bitset<8>(val).to_string(); // converting to binary, might want to look over this
    std::string binA = std::bitset<8>(cpu->AF.high_8).to_string();
    ASSERT_EQ(binVal, binA);
    ASSERT_EQ(cpu->AF.low_8 & 0x40, 0x40);
    //TODO uncertain if H-flag is supposed to be set here or not, not set atm.


    mmu->write(cpu->PC, 0x80); //ADD B
    cpu->setA(255);
    cpu->setB(1);
    val = cpu->AF.high_8 + cpu->BC.high_8;
    cpu->execute_cycle();
    ASSERT_EQ(cpu->AF.high_8, val);
    ASSERT_EQ(cpu->AF.low_8, 0xB0); //Z,H and C.
    mmu->write(cpu->PC, 0x88); //ADC B
    cpu->setA(15);
    cpu->setB(5);
    val = cpu->AF.high_8 + cpu->BC.high_8;
    cpu->execute_cycle();
    ASSERT_EQ(cpu->AF.high_8, val + 1);
    ASSERT_EQ(cpu->AF.low_8, 0x20); // Only H flag.

    //setting C-flag for next test.
    mmu->write(cpu->PC, 0x80); //ADD B
    cpu->setA(255);
    cpu->setB(1);
    val = cpu->AF.high_8 + cpu->BC.high_8;
    cpu->execute_cycle();
    //C-flag set and rdy for SBC test.
    mmu->write(cpu->PC, 0x98);//SBC B
    cpu->setA(10);
    cpu->setB(5);
    val = cpu->AF.high_8 - cpu->BC.high_8;
    cpu->execute_cycle();
    ASSERT_EQ(cpu->AF.high_8, val - 1);
    ASSERT_EQ(cpu->AF.low_8 & 0x40, 0x40);
    //TODO uncertain if H-flag is supposed to be set here or not, not set atm.

    mmu->write(cpu->PC, 0x04); // INC B
    int8_t prevB = cpu->BC.high_8;
    cpu->execute_cycle();
    ASSERT_EQ(cpu->BC.high_8, prevB + 1);

    mmu->write(cpu->PC, 0x05); // DEC B
    prevB = cpu->BC.high_8;
    cpu->execute_cycle();
    ASSERT_EQ(cpu->BC.high_8, prevB - 1);
    ASSERT_EQ(cpu->AF.low_8 & 0x40, 0x40);

    mmu->write(cpu->PC, 0x33); // INC SP
    uint16_t prevSP = cpu->SP.all_16;
    cpu->execute_cycle();
    ASSERT_EQ(cpu->SP.all_16, prevSP + 1);

    mmu->write(cpu->PC, 0x3B); // DEC SP
    prevSP = cpu->SP.all_16;
    cpu->execute_cycle();
    ASSERT_EQ(cpu->SP.all_16, prevSP - 1);
}
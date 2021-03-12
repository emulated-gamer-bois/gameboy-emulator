//
// Created by davidm on 2021-02-12.
//

#include <memory>
#include "gtest/gtest.h"
#include "../src/gameboy/CPU.h"

TEST(CPU, Execute_NOP_Instruction) {
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<CPU> cpu(new CPU(0x0000, 0xFFFE, mmu));

    // Disable boot ROM
    mmu->write(0xff50, 0x01);

    mmu->write_GAME_ROM_ONLY_IN_TESTS(0x00, 0x00);
    cpu->execute_instruction();

    ASSERT_EQ(mmu->read(0x00), 0x00);
    ASSERT_EQ(cpu->PC, 0x01);
}

TEST(CPU, Execute_LD_SP_D16_Instruction) {
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<CPU> cpu(new CPU(0x0000, 0xFFFE, mmu));

    // Disable boot ROM
    mmu->write(0xff50, 0x01);

    mmu->write_GAME_ROM_ONLY_IN_TESTS(0x00, 0x31);//pc+3
    mmu->write_GAME_ROM_ONLY_IN_TESTS(0x01, 0xFF);//pc+1
    mmu->write_GAME_ROM_ONLY_IN_TESTS(0x02, 0xAA);//pc+1
    cpu->execute_instruction();

    ASSERT_EQ(cpu->PC, 0x03);
    ASSERT_EQ(cpu->SP.all_16, 0xAAFF);
}

TEST(CPU, FUNDAMENTAL_FUNCTIONS) {
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<CPU> cpu(new CPU(0x00, 0xC100, mmu));

    // Disable boot ROM
    mmu->write(0xff50, 0x01);

    cpu->setZNFlags(0x00, false);
    ASSERT_EQ(cpu->F.z, 1);
    ASSERT_EQ(cpu->F.n, 0);


    cpu->setZNFlags(0x01, true);
    ASSERT_EQ(cpu->F.z, 0);
    ASSERT_EQ(cpu->F.n, 1);

    cpu->setHFlag(0x0F, 0x01, false, 0);
    ASSERT_EQ(cpu->F.h, 1);

    cpu->setHFlag(0x0D, 0x01, false, 0);
    ASSERT_EQ(cpu->F.h, 0);

    cpu->setHFlag(0x10, 0x01, true, 0);
    ASSERT_EQ(cpu->F.h, 1);

    cpu->setHFlag(0xF3, 0x11, true, 0);
    ASSERT_EQ(cpu->F.h, 0);

    cpu->setCFlag(0xFF, 0x01, false);
    ASSERT_EQ(cpu->F.c, 1);

    cpu->setCFlag(0xFF, 0x81, true);
    ASSERT_EQ(cpu->F.c, 0);

    cpu->orA(0x55);
    ASSERT_EQ(cpu->A, 0x55);

    cpu->xorA(0x04);
    ASSERT_EQ(cpu->A, 0x51);

    cpu->andA(0x4F);
    ASSERT_EQ(cpu->A, 0x41);

    cpu->rlc(cpu->A);
    ASSERT_EQ(cpu->A, 0x82);
    ASSERT_EQ(cpu->F.c, 0);


    cpu->rlc(cpu->A);
    ASSERT_EQ(cpu->A, 0x05);
    ASSERT_EQ(cpu->F.c, 1);


    cpu->rl(cpu->A);
    ASSERT_EQ(cpu->A, 0x0B);
    ASSERT_EQ(cpu->F.c, 0);


    cpu->rrc(cpu->A);
    ASSERT_EQ(cpu->A, 0x85);
    ASSERT_EQ(cpu->F.c, 1);


    cpu->rr(cpu->A);
    ASSERT_EQ(cpu->A, 0xC2);
    ASSERT_EQ(cpu->F.c, 1);


    cpu->setA(0x05);
    cpu->setB(0x0E);
    uint8_t val = cpu->A + cpu->BC.high_8;
    cpu->addA(cpu->BC.high_8, false);
    ASSERT_EQ(cpu->A, val);
    ASSERT_EQ(cpu->F.all_8 & 0xF0, 0x20); // Only H-flag set.

    cpu->setA(8);
    cpu->setB(8);
    cpu->subA(cpu->BC.high_8, false);
    ASSERT_EQ(0x00, cpu->A);
    ASSERT_EQ(cpu->F.all_8 & 0xF0, 0xC0); // Z,N set

    cpu->setA(255);
    cpu->setB(1);
    val = cpu->A + cpu->BC.high_8;
    cpu->addA(cpu->BC.high_8, false);
    ASSERT_EQ(cpu->A, val);
    ASSERT_EQ(cpu->F.all_8, 0xB0); //Z,H and C.

    cpu->setA(15);
    cpu->setB(5);
    val = cpu->A + cpu->BC.high_8;
    cpu->addA(cpu->BC.high_8, true);
    ASSERT_EQ(cpu->A, val + 1);
    ASSERT_EQ(cpu->F.all_8, 0x20); // Only H flag.

    //setting C-flag for next test.
    cpu->F.all_8 |= 0x10;

    cpu->setA(10);
    cpu->setB(5);
    val = cpu->A - cpu->BC.high_8;
    cpu->subA(cpu->BC.high_8, true);
    ASSERT_EQ(cpu->A, val - 1);
    ASSERT_EQ(cpu->F.all_8 & 0xF0, 0x40); //N set

    uint16_t addr = 0xC123;
    uint8_t data = 137;
    mmu->write(addr, data);
    uint8_t prevVal = mmu->read(addr);
    cpu->incrementAddr(addr);
    uint8_t newVal = mmu->read(addr);
    ASSERT_EQ(prevVal + 1, newVal);

    prevVal = mmu->read(addr);
    cpu->decrementAddr(addr);
    newVal = mmu->read(addr);
    ASSERT_EQ(prevVal - 1, newVal);

    uint8_t prevB = cpu->BC.high_8;
    cpu->increment8(cpu->BC.high_8);
    ASSERT_EQ(cpu->BC.high_8, prevB + 1);

    cpu->BC.high_8 = 0x10;
    cpu->decrement8(cpu->BC.high_8);
    ASSERT_EQ(cpu->BC.high_8, 0x0F);
    ASSERT_EQ(cpu->F.all_8 & 0xE0, 0x60);

    uint16_t prevSP = cpu->SP.all_16;
    cpu->increment16(cpu->SP.all_16);
    ASSERT_EQ(cpu->SP.all_16, prevSP + 1);

    prevSP = cpu->SP.all_16;
    cpu->decrement16(cpu->SP.all_16);
    ASSERT_EQ(cpu->SP.all_16, prevSP - 1);

    cpu->loadIm16(0xFE << 8 | 0xDC, cpu->HL);
    ASSERT_EQ(cpu->HL.all_16, 0xFEDC);
    cpu->loadIm8(cpu->HL.low_8, 0xBA);
    ASSERT_EQ(cpu->HL.all_16, 0xFEBA);

    //RAM: 0xC000 to 0xE000
    mmu->write(0xC001, 0x98);
    cpu->loadImp(0xC001, cpu->HL.high_8);
    ASSERT_EQ(cpu->HL.all_16, 0x98BA);

    cpu->storeAddr(0xC002, cpu->HL.low_8);
    ASSERT_EQ(mmu->read(0xC002), 0xBA);

    cpu->setA(0xAB);
    cpu->compareA(0xAB);
    ASSERT_EQ(cpu->F.all_8 & 0xF0, 0xC0);

    cpu->compareA(0xA0);
    ASSERT_EQ(cpu->F.all_8 & 0xF0, 0x40);

    //RAM: 0xC000 to 0xE000
    prevSP = cpu->SP.all_16;
    cpu->HL.all_16 = 0x98BA;
    cpu->pushReg(cpu->HL);
    cpu->HL.all_16 = 0x1234;
    ASSERT_NE(cpu->HL.all_16, 0x98BA);
    ASSERT_EQ(cpu->SP.all_16, prevSP - 2);
    cpu->popReg(cpu->HL);
    ASSERT_EQ(cpu->HL.all_16, 0x98BA);
    ASSERT_EQ(cpu->SP.all_16, prevSP);

    cpu->jump(0xC123);
    ASSERT_EQ(cpu->PC, 0xC123);
    cpu->F.all_8 |= 0x80;
    cpu->jumpZ(0xD123, true);
    ASSERT_EQ(cpu->PC, 0xD123);
    cpu->jumpZ(0xC123, false);
    ASSERT_EQ(cpu->PC, 0xD123);
    cpu->F.all_8 &= 0x7F;
    cpu->jumpZ(0xC123, false);
    ASSERT_EQ(cpu->PC, 0xC123);
    cpu->jumpZ(0xB123, true);
    ASSERT_EQ(cpu->PC, 0xC123);

    cpu->F.all_8 |= 0x10;
    cpu->jumpC(0xD123, true);
    ASSERT_EQ(cpu->PC, 0xD123);
    cpu->jumpC(0xC123, false);
    ASSERT_EQ(cpu->PC, 0xD123);
    cpu->F.all_8 &= 0xEF;
    cpu->jumpC(0xC123, false);
    ASSERT_EQ(cpu->PC, 0xC123);
    cpu->jumpC(0xB123, true);
    ASSERT_EQ(cpu->PC, 0xC123);

    cpu->jumpRelative(0x02);
    ASSERT_EQ(cpu->PC, 0xC125);
    cpu->jumpRelative(-0x02);
    ASSERT_EQ(cpu->PC, 0xC123);
    cpu->F.all_8 |= 0x80;
    cpu->jumpRelativeZ(0x10, true);
    ASSERT_EQ(cpu->PC, 0xC133);
    cpu->jumpRelativeZ(0x10, false);
    ASSERT_EQ(cpu->PC, 0xC133);
    cpu->F.all_8 &= 0x7F;
    cpu->jumpRelativeZ(-0x20, false);
    ASSERT_EQ(cpu->PC, 0xC113);
    cpu->jumpRelativeZ(0x10, true);
    ASSERT_EQ(cpu->PC, 0xC113);

    cpu->F.all_8 |= 0x10;
    cpu->jumpRelativeC(0x10, true);
    ASSERT_EQ(cpu->PC, 0xC123);
    cpu->jumpRelativeC(0x10, false);
    ASSERT_EQ(cpu->PC, 0xC123);
    cpu->F.all_8 &= 0xEF;
    cpu->jumpRelativeC(-0x20, false);
    ASSERT_EQ(cpu->PC, 0xC103);
    cpu->jumpRelativeC(0x10, true);
    ASSERT_EQ(cpu->PC, 0xC103);

    auto prevPC = cpu->PC;
    prevSP = cpu->SP.all_16;
    cpu->call(0xCD, 0xAB);
    ASSERT_EQ(cpu->PC, 0xABCD);
    ASSERT_EQ(cpu->SP.all_16, prevSP - 2);
    cpu->ret(false);
    ASSERT_EQ(cpu->PC, prevPC);
    ASSERT_EQ(cpu->SP.all_16, prevSP);

    cpu->F.all_8 |= 0x80;
    cpu->callZ(0xCD, 0xAB, false);
    ASSERT_EQ(cpu->PC, prevPC);
    ASSERT_EQ(cpu->SP.all_16, prevSP);
    cpu->callZ(0xCD, 0xAB, true);
    ASSERT_EQ(cpu->PC, 0xABCD);
    ASSERT_EQ(cpu->SP.all_16, prevSP - 2);
    cpu->retZ(false);
    ASSERT_EQ(cpu->PC, 0xABCD);
    ASSERT_EQ(cpu->SP.all_16, prevSP - 2);
    cpu->retZ(true);
    ASSERT_EQ(cpu->PC, prevPC);
    ASSERT_EQ(cpu->SP.all_16, prevSP);

    cpu->F.all_8 &= 0xEF;
    cpu->callC(0xCD, 0xAB, true);
    ASSERT_EQ(cpu->PC, prevPC);
    ASSERT_EQ(cpu->SP.all_16, prevSP);
    cpu->callC(0xCD, 0xAB, false);
    ASSERT_EQ(cpu->PC, 0xABCD);
    ASSERT_EQ(cpu->SP.all_16, prevSP - 2);
    cpu->retC(true);
    ASSERT_EQ(cpu->PC, 0xABCD);
    ASSERT_EQ(cpu->SP.all_16, prevSP - 2);
    cpu->retC(false);
    ASSERT_EQ(cpu->PC, prevPC);
    ASSERT_EQ(cpu->SP.all_16, prevSP);

    cpu->reset(3);
    ASSERT_EQ(cpu->PC, 0x0018);
    cpu->ret(false);
    ASSERT_EQ(cpu->PC, prevPC);

    cpu->setA(0xF0);
    cpu->bit(7, cpu->A);
    ASSERT_EQ(cpu->F.all_8 & 0xE0, 0x20);
    cpu->setA(0xF0);
    cpu->bit(3, cpu->A);
    ASSERT_EQ(cpu->F.all_8 & 0xE0, 0xA0);

    cpu->HL.all_16 = 0x4C00;
    auto tempz = cpu->F.z;
    cpu->addHL(cpu->HL);
    ASSERT_EQ(cpu->HL.all_16, 0x9800);
    ASSERT_EQ(cpu->F.z, tempz);
    ASSERT_EQ(cpu->F.h, 1);
    ASSERT_EQ(cpu->F.c, 0);
    cpu->F.all_8 = 0;
    cpu->SP.all_16 = 0xFFFF;

    cpu->addSignedToRegPair(cpu->SP, 0x01);
    ASSERT_EQ(cpu->SP.all_16, 0x0000);
    ASSERT_EQ(cpu->F.all_8 & 0xF0, 0x30);

    cpu->BC.high_8 = 0xFF;
    cpu->F.all_8 = 0;
    cpu->sla(cpu->BC.high_8);
    ASSERT_EQ(cpu->BC.high_8, 0xFE);
    ASSERT_EQ(cpu->F.all_8 & 0xF0, 0x10);

    cpu->BC.high_8 = 0xC8;
    cpu->F.all_8 = 0;
    cpu->sla(cpu->BC.high_8);
    ASSERT_EQ(cpu->BC.high_8, 0x90);
    ASSERT_EQ(cpu->F.all_8 & 0xF0, 0x10);

    cpu->BC.high_8 = 0xD8;
    cpu->F.all_8 = 0;
    cpu->sra(cpu->BC.high_8);
    ASSERT_EQ(cpu->BC.high_8, 0xEC);
    ASSERT_EQ(cpu->F.all_8 & 0xF0, 0);

    cpu->BC.high_8 = 0x81;
    cpu->F.all_8 = 0;
    cpu->sra(cpu->BC.high_8);
    ASSERT_EQ(cpu->BC.high_8, 0xC0);
    ASSERT_EQ(cpu->F.all_8 & 0xF0, 0x10);

    cpu->BC.high_8 = 0x81;
    cpu->F.all_8 = 0;
    cpu->srl(cpu->BC.high_8);
    ASSERT_EQ(cpu->BC.high_8, 0x40);
    ASSERT_EQ(cpu->F.all_8 & 0xF0, 0x10);

    cpu->A = 0;
    cpu->compareA(0);
    ASSERT_EQ(cpu->F.all_8 & 0xF0, 0xC0);

    cpu->HL.all_16 = 0x1;
    cpu->SP.all_16 = 0x7FFF;
    cpu->addHL(cpu->SP);
    ASSERT_EQ(cpu->HL.all_16, 0x8000);
    ASSERT_EQ(cpu->F.all_8 & 0x70, 0x20);

    cpu->setA(0);
    cpu->F.c = 1;
    cpu->subA(0xFF, true);
    ASSERT_EQ(cpu->F.c, 1);
  
    cpu->A = 0x66;
    cpu->addA(0x66, false);
    ASSERT_EQ(cpu->A, 0xCC);
    cpu->daa();
    ASSERT_EQ(cpu->A, 0x32);
    cpu->A = 0xF5;
    cpu->subA(0x60, false);
    ASSERT_EQ(cpu->A, 0x95);
    cpu->daa();
    ASSERT_EQ(cpu->A, 0x95);
    cpu->addA(0x60, false);
    cpu->daa();
    ASSERT_EQ(cpu->A, 0x55);

    cpu->F.c = 1;
    cpu->F.h=1;
    cpu->F.n=1;
    cpu->ccf();
    ASSERT_EQ(cpu->F.c, 0);
    cpu->ccf();
    cpu->F.h=0;
    cpu->F.n=0;
    ASSERT_EQ(cpu->F.c, 1);
    cpu->A=0x00;
    cpu->cpl();
    cpu->F.h=1;
    cpu->F.n=1;
    ASSERT_EQ(cpu->A,0xFF);
    cpu->A=0xFF;
    cpu->cpl();
    ASSERT_EQ(cpu->A,0x00);
}

TEST(CPU, sixteen_bit_ops) {
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<CPU> cpu(new CPU(0x00, 0xC100, mmu));
    cpu->HL.low_8 = 0xFD;
    cpu->bit(1, cpu->HL.low_8);
    ASSERT_EQ(cpu->F.n, 0);
    ASSERT_EQ(cpu->F.h, 1);
    ASSERT_EQ(cpu->F.z, 1);
    cpu->HL.low_8 = 0;
    cpu->set(0, cpu->HL.low_8);
    ASSERT_EQ(cpu->HL.low_8, 0x01);
    cpu->set(5, cpu->HL.low_8);
    ASSERT_EQ(cpu->HL.low_8, 0x21);
    cpu->res(0, cpu->HL.low_8);
    ASSERT_EQ(cpu->HL.low_8, 0x20);
    cpu->res(5, cpu->HL.low_8);
    ASSERT_EQ(cpu->HL.low_8, 0x00);

    ASSERT_EQ(cpu->swapBits(0xF0), 0x0F);
    ASSERT_EQ(cpu->swapBits(0xAB), 0xBA);
}

/*TEST(CPU, TIMING) {
    int cycles[] = {1,3,2,2,1,1,2,1,5,2,2,2,1,1,2,1,
                     0,3,2,2,1,1,2,1,3,2,2,2,1,1,2,1,
                     2,3,2,2,1,1,2,1,2,2,2,2,1,1,2,1,
                     2,3,2,2,3,3,3,1,2,2,2,2,1,1,2,1,
                     1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
                     1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
                     1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
                     2,2,2,2,2,2,0,2,1,1,1,1,1,1,2,1,
                     1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
                     1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
                     1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
                     1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
                     2,3,3,4,3,4,2,4,2,4,3,0,3,6,2,4,
                     2,3,3,0,3,4,2,4,2,4,3,0,3,0,2,4,
                     3,3,2,0,0,4,2,4,4,1,4,0,0,0,2,4,
                     3,3,2,1,0,4,2,4,3,2,4,1,0,0,2,4};

    int cb_cycles[] = {2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
                      2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
                      2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
                      2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
                      2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
                      2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
                      2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
                      2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
                      2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
                      2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
                      2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
                      2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
                      2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
                      2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
                      2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
                      2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2};

    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<CPU> cpu(new CPU(0x100, 0xFFFE, mmu));

    // Disable boot ROM
    mmu->write(0xff50, 0x01);

    for(auto i = 0; i < 0x76; i++) {
        mmu->write(cpu->PC, i);
        if(cpu->update() != cycles[i]) {
            std::cout << i << std::endl;
            ASSERT_EQ(1,2);
        }
    }

    // Skips the HALT function

    for(auto i = 0x77; i < 0xCB; i++) {
        mmu->write(cpu->PC, i);
        ASSERT_EQ(cpu->update(), cycles[i]);
    }

    for(auto i = 0x00; i < 0x100; i++) {
        mmu->write(cpu->PC, 0xCB);
        mmu->write(cpu->PC + 1, i);
        ASSERT_EQ(cpu->update(), cb_cycles[i]);
    }

    for(auto i = 0xCC; i < 0x100; i++) {
        mmu->write(cpu->PC, i);
        ASSERT_EQ(cpu->update(), cycles[i]);
    }
}*/
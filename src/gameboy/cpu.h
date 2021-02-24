//
// Created by davidm on 2021-02-12.
//

#ifndef LAME_BOY_CPU_H
#define LAME_BOY_CPU_H

#include <iostream>
#include <cstdint>
#include <memory>
#include <cstdint>
#include "register_pair.h"
#include "mmu.h"
#include "Flags.h"

#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test

class CPU {
public:
    CPU(uint16_t PC, uint16_t SP, std::shared_ptr<MMU> memory);

    /**
     * Fetches, decodes and executes the instruction at location PC
     */
    void execute_instruction();

private:
    //Registers
    uint16_t PC;
    RegisterPair SP;
    RegisterPair A;
    RegisterPair BC;
    RegisterPair DE;
    RegisterPair HL;
    //Flags
    Flags F;
    //Memory
    std::shared_ptr<MMU> memory;

    //Flag management
    void setZNFlags(uint8_t, bool);
    void setHFlag(uint8_t a, uint8_t b);
    void setCFlag(uint16_t a,uint16_t b);

    //Setting registers
    void setA(uint8_t val){ A.high_8=val;};
    void setB(uint8_t val){ BC.high_8=val;};

    //Register arithmetics
    void addA(uint8_t value, bool withCarry);
    void subA(uint8_t value, bool withCarry);
    void increment8(uint8_t &reg);
    void increment16(uint16_t &reg);
    void decrement8(uint8_t &reg);
    void decrement16(uint16_t &reg);

    //Bitwise operations
    void andA(uint8_t value);
    void xorA(uint8_t value);
    void orA(uint8_t value);
    void rlc(uint8_t &reg);
    void rl(uint8_t &reg);
    void rrc(uint8_t &reg);
    void rr(uint8_t &reg);

    //Read and write memory
    void storeAddr(uint16_t addr, uint8_t value);
    void loadImp(uint16_t addr, uint8_t &reg);
    void loadIm16(uint16_t value, RegisterPair &reg);
    void loadIm8(uint8_t &reg, uint8_t firstByte);

    //Stack functions
    void popReg(RegisterPair &reg);
    void pushReg(RegisterPair &reg);

    //Jump and Branch
    //Jump (aka JP)   = Jump to the specified address
    //Jump Relative (aka JR) = Jump a specified number of steps from PC
    void jump(uint16_t addr);
    void jumpZ(uint16_t addr, bool if_one);
    void jumpC(uint16_t addr, bool if_one);
    void jumpRelative(int8_t steps);
    void jumpRelativeZ(int8_t steps, bool if_one);
    void jumpRelativeC(int8_t steps, bool if_one);

    //Call subroutines and return
    void ret(bool from_interrupt);
    void retZ(bool if_one);
    void retC(bool if_one);
    void call(uint8_t firstByte, uint8_t secondByte);
    void callZ(uint8_t firstByte, uint8_t secondByte, bool if_one);
    void callC(uint8_t firstByte, uint8_t secondByte, bool if_one);

    //Other
    void reset(uint8_t nth_byte);
    void compareA(uint8_t value);
    void bit(uint8_t bit_nr, uint8_t value);
    uint8_t read_and_inc_pc();
    uint16_t read16_and_inc_pc();

    FRIEND_TEST(CPU, Execute_NOP_Instruction);
    FRIEND_TEST(CPU, Execute_LD_SP_D16_Instruction);
    FRIEND_TEST(CPU, FUNDAMENTAL_FUNCTIONS);
    FRIEND_TEST(CPU, sixteen_bit_ops);
    int CB_ops();

    void res(uint8_t bit_nr, uint8_t &reg);

    void set(uint8_t bit_nr, uint8_t &reg);
};


#endif //LAME_BOY_CPU_H

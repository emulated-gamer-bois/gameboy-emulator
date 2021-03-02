//
// Created by davidm on 2021-02-12.
//

#pragma once

#include <iostream>
#include <cstdint>
#include <memory>
#include <cstdint>
#include "RegisterPair.h"
#include "MMU.h"
#include "Flags.h"

#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test

class CPU {
public:
    CPU(uint16_t PC, uint16_t SP, std::shared_ptr<MMU> memory);

    /**
     * Fetches, decodes and executes the instruction at location PC
     * @returns amount of machine cycles operation takes.
     */
    int execute_instruction();
    void cpu_dump();

private:
    //Registers
    uint16_t PC;
    RegisterPair SP;
    uint8_t  A;
    RegisterPair BC;
    RegisterPair DE;
    RegisterPair HL;
    //Flags
    Flags F;
    //Memory
    std::shared_ptr<MMU> memory;

    //Flag management
    void setZNFlags(uint8_t value, bool subtraction);
    void setHFlag(uint8_t a, uint8_t b, bool subtraction, uint8_t cFlag);
    void setCFlag(uint16_t a,uint16_t b, bool subtraction);

    //Setting registers
    void setA(uint8_t val){ A=val;};
    void setB(uint8_t val){ BC.high_8=val;};

    //Register arithmetics
    void addA(uint8_t value, bool withCarry);
    void subA(uint8_t value, bool withCarry);
    void incrementAddr(uint16_t addr);
    void increment8(uint8_t &reg);
    void increment16(uint16_t &reg);
    void decrementAddr(uint16_t addr);
    void decrement8(uint8_t &reg);
    void decrement16(uint16_t &reg);
    void add_8bit(uint8_t &reg, uint8_t b, bool withCarry);
    void addHL(RegisterPair reg);
    void addSignedToRegPair(RegisterPair &regPair, int8_t value);

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
    bool jumpZ(uint16_t addr, bool if_one);
    bool jumpC(uint16_t addr, bool if_one);
    void jumpRelative(int8_t steps);
    bool jumpRelativeZ(int8_t steps, bool if_one);
    bool jumpRelativeC(int8_t steps, bool if_one);

    //Call subroutines and return
    void ret(bool from_interrupt);
    bool retZ(bool if_one);
    bool retC(bool if_one);
    void call(uint8_t firstByte, uint8_t secondByte);
    bool callZ(uint8_t firstByte, uint8_t secondByte, bool if_one);
    bool callC(uint8_t firstByte, uint8_t secondByte, bool if_one);

    //16 bit operations
    int CB_ops();
    void bit(uint8_t bit_nr, uint8_t value);
    void res(uint8_t bit_nr, uint8_t &reg);
    void sla(uint8_t &reg);
    void sra(uint8_t &reg);
    void srl(uint8_t &reg);
    void set(uint8_t bit_nr, uint8_t &reg);
    uint8_t swapBits(uint8_t value);

    //Other
    void reset(uint8_t nth_byte);
    void compareA(uint8_t value);
    uint8_t read_and_inc_pc();
    uint16_t read16_and_inc_pc();

    FRIEND_TEST(CPU, Execute_NOP_Instruction);
    FRIEND_TEST(CPU, Execute_LD_SP_D16_Instruction);
    FRIEND_TEST(CPU, FUNDAMENTAL_FUNCTIONS);
    FRIEND_TEST(CPU, sixteen_bit_ops);
    FRIEND_TEST(PPU, Print_test_rom);
    FRIEND_TEST(PPU, g_tile_rom);

};

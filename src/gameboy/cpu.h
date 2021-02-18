//
// Created by davidm on 2021-02-12.
//

#ifndef LAME_BOY_CPU_H
#define LAME_BOY_CPU_H

#include <iostream>
#include <memory>
#include <cstdint>
#include "register_pair.h"
#include "gtest/gtest.h"

/**
 * Temporary class until the actual MMU is created
 */
class MMU {
public:
    MMU() {
        memory = new u_int16_t[0xF000];
    }

    ~MMU() {
        delete[] memory;
    }

    u_int16_t read(int addr) { return memory[addr]; }

    void write(int addr, u_int16_t value) { memory[addr] = value; }

private:
    u_int16_t *memory;
};

class CPU {
public:
    CPU(uint16_t, uint16_t, std::shared_ptr<MMU>);

    /**
     * Fetches, decodes and executes the instruction at location PC
     */
    void execute_cycle();

private:
    //Registers
    u_int16_t PC;
    RegisterPair SP;
    RegisterPair AF;
    RegisterPair BC;
    RegisterPair DE;
    RegisterPair HL;
    //Memory
    std::shared_ptr<MMU> memory;

    //Flag management
    void setZNFlags(uint8_t, bool);
    void setHFlag(uint8_t a, uint8_t b);
    void setCFlag(uint8_t a, uint8_t b);

    //Setting registers
    void setA(uint8_t val){ AF.high_8=val;};
    void setB(uint8_t val){ BC.high_8=val;};

    //Register arithmetics
    void addA(uint8_t value, bool withCarry);
    void subA(uint8_t value, bool withCarry);
    void increment8(uint8_t &addr);
    void increment16(uint16_t &reg);
    void decrement8(uint8_t &addr);
    void decrement16(uint16_t &addr);

    //Bitwise operations
    void andA(uint8_t);
    void xorA(uint8_t);
    void orA(uint8_t);
    void rlc(uint8_t &reg);
    void rl(uint8_t &reg);
    void rrc(uint8_t &reg);
    void rr(uint8_t &reg);

    //Read and write memory
    void storeAddr(uint16_t addr, uint8_t value);
    void loadImp(uint8_t &reg, uint16_t addr);
    void loadIm16(uint8_t firstByte, uint8_t secondByte, RegisterPair &reg);
    void loadIm8(uint8_t firstByte, uint8_t &reg);

    //Stack functions
    void popReg(RegisterPair &reg);
    void pushReg(RegisterPair &reg);

    //Jump and Branch
    //Jump (aka JP)   = Jump to the specified address
    //Branch (aka JR) = Jump a specified number of steps from PC
    void jump(uint16_t addr);
    void jumpZ(uint16_t addr, bool if_one);
    void branch(int8_t steps);
    void branchZ(int8_t steps, bool if_one);

    //Other
    void compareA(uint8_t value);

    FRIEND_TEST(CPU, FUNDAMENTAL_FUNCTIONS);
};


#endif //LAME_BOY_CPU_H

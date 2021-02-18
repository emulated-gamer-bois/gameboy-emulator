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

/**
 * Temporary class until the actual MMU is created
 */
class MMU {
public:
    MMU() {
        memory = new uint16_t[0xF000];
    }

    ~MMU() {
        delete[] memory;
    }

    uint16_t read(int addr) { return memory[addr]; }

    void write(int addr, uint16_t value) { memory[addr] = value; }

private:
    uint16_t *memory;
};

class CPU {
public:
    CPU(uint16_t, std::shared_ptr<MMU>);

    /**
     * Fetches, decodes and executes the instruction at location PC
     */
    void execute_cycle();

    /**
     * Returns value of PC for testing purposes
     */
    uint16_t getPC() { return PC; }

    /**
     * Returns value of SP for testing purposes
     */
    uint16_t getSP() { return SP.all_16; }

    uint8_t getFlags() {return AF.low_8; }
    //TODO: Should be private using FRIEND_TEST(CPU, FUNDAMENTAL_FUNCTIONS)
    //https://stackoverflow.com/questions/2396370/how-to-make-google-test-classes-friends-with-my-classes
    uint8_t getB()  {return BC.high_8; }
    uint8_t getA()  {return AF.high_8; }
    void setA(uint8_t val){ AF.high_8=val;};
    void setB(uint8_t val){ BC.high_8=val;};
    void setZNFlags(uint8_t, bool);
    void setHFlag(uint8_t a, uint8_t b);

    void setCFlag(uint8_t a, uint8_t b);


    void addA(uint8_t value, bool withCarry);
    void subA(uint8_t value, bool withCarry);


    void andA(uint8_t);
    void xorA(uint8_t);
    void orA(uint8_t);
private:
    //Registers
    uint16_t PC;
    RegisterPair SP;
    RegisterPair AF;
    RegisterPair BC;
    RegisterPair DE;
    RegisterPair HL;
    //Memory
    std::shared_ptr<MMU> memory;


    void storeAddr(uint16_t addr, uint8_t value);
    void increment8(uint8_t &addr);
    void decrement8(uint8_t &addr);
    void decrement16(uint16_t &addr);

    void rlc(uint8_t &reg);

    void rl(uint8_t &reg);

    void rrc(uint8_t &reg);

    void rr(uint8_t &reg);
    uint16_t read_and_inc_pc();

    void loadImp(uint16_t addr, uint8_t &reg);
};


#endif //LAME_BOY_CPU_H

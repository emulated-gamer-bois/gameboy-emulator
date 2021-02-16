//
// Created by davidm on 2021-02-12.
//

#ifndef LAME_BOY_CPU_H
#define LAME_BOY_CPU_H

#include <iostream>
#include <memory>
#include <cstdint>
#include "register_pair.h"

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
    CPU(u_int16_t, std::shared_ptr<MMU>);

    /**
     * Fetches, decodes and executes the instruction at location PC
     */
    void execute_cycle();

    /**
     * Returns value of PC for testing purposes
     */
    u_int16_t getPC() { return PC; }

    /**
     * Returns value of SP for testing purposes
     */
    u_int16_t getSP() { return SP.all_16; }

    void setZNFlags(uint8_t, bool);

    uint8_t getFlags() { return AF.low_8; }

    void setHFlag(uint8_t a, uint8_t b);

    void setCFlag(uint8_t a, uint8_t b);

    void addImp(uint16_t addr);


    void addImp(uint8_t value);

//Here for testing only


    void addImp(uint8_t value, bool withCarry);
    void subImp(uint8_t value, bool withCarry);


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


    void storeAddr(uint16_t addr, uint8_t value);
    void loadImp(uint8_t &reg, uint16_t addr);
    void increment8(uint8_t &addr, bool setFlags);
    void increment16(uint16_t &addr);
    void decrement8(uint8_t &addr, bool setFlags);
    void decrement16(uint8_t &addr);
    void decrement16(uint16_t &addr);
};


#endif //LAME_BOY_CPU_H

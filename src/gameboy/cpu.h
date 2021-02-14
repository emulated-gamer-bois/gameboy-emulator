//
// Created by davidm on 2021-02-12.
//

#ifndef LAME_BOY_CPU_H
#define LAME_BOY_CPU_H

#include <iostream>
#include <memory>

/**
 * Temporary class until the actual MMU is created
 */
class MMU {
public:
    MMU() {
        memory = new u_int16_t[0xF000];
    }
    ~MMU(){
        delete[] memory;
    }

    u_int16_t read(int addr) {return memory[addr];}
    void write(int addr, u_int16_t value) {memory[addr] = value; }
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
     u_int16_t getPC() {return PC;}
private:
    u_int16_t PC;
    std::shared_ptr<MMU> memory;
};


#endif //LAME_BOY_CPU_H

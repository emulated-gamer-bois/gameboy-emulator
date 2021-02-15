//
// Created by davidm on 2021-02-12.
//

#ifndef LAME_BOY_CPU_H
#define LAME_BOY_CPU_H

#include <iostream>
#include <memory>
#include "register_pair.h"
#include "mmu.h"


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
     uint16_t getPC() {return PC;}

     /**
      * Returns value of SP for testing purposes
      */
     uint16_t  getSP() {return SP.all_16; }
private:
    //Registers
    uint16_t PC;
    RegisterPair SP;

    //Memory
    std::shared_ptr<MMU> memory;
};


#endif //LAME_BOY_CPU_H

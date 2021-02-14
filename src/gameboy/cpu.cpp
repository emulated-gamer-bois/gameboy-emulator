//
// Created by davidm on 2021-02-12.
//

#include "cpu.h"

CPU::CPU(u_int16_t PC, std::shared_ptr<MMU> mmu) {
    this->PC = PC;
    this->memory = mmu;
}

void nop() {}

void CPU::execute_cycle() {
    switch (memory->read(PC++)) {
        case 0x00:
            nop();
            break;
        default:
            nop();
            break;
    }
}

//
// Created by davidm on 2021-02-12.
//

#include "cpu.h"

CPU::CPU(u_int16_t, MMU &memory) {
    this->PC = PC;
    this->memory = &memory;
}

void nop() {}

void CPU::execute_cycle() {
    switch (memory->read(PC++)) {
        case 0x00:
            nop();
            break;
        default:
            nop();
    }
}

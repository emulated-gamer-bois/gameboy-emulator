//
// Created by davidm on 2021-02-12.
//

#include "cpu.h"

CPU::CPU(u_int16_t PC, std::shared_ptr<MMU> mmu) {
    this->PC = PC;
    this->memory = mmu;
}

void nop() {}

void ldsp(u_int8_t first_byte,u_int8_t second_byte, RegisterPair &SP) {
    SP.low_8 = first_byte;
    SP.high_8 = second_byte;
}

void CPU::execute_cycle() {
    switch (memory->read(PC++)) {
        case 0x00: //NOP
            nop();
            break;
        case 0x31: // LD SP, d16
            ldsp(memory->read(PC), memory->read(PC+1), SP);
            PC += 2;
            break;
        default:
            nop();
            break;
    }
}

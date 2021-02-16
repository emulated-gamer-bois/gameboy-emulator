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

void CPU::setZNFlags(uint8_t newValue, bool subtraction) {
    //Sets the Z flag to 1 if newValue == 0
    AF.low_8 &= 0x7F;
    AF.low_8 |= newValue == 0x00 ? 0x80 : 0x00;

    //Set the N flag if operation is subtraction
    AF.low_8 |=  subtraction ? 0x40 : 0x00;
    AF.low_8 &= !subtraction ? 0xBF : 0xFF;
}

void CPU::setHFlag(uint8_t a, uint8_t b) {
    // Sets the H flag if carry from bit 3 to bit 4
    auto HFlag = (((a & 0x0F) + ( b & 0x0F)) & 0x10) << 0x01;
    AF.low_8 &= 0xDF;
    AF.low_8 |= HFlag;
}

void CPU::setCFlag(uint8_t a, uint8_t b) {
    // Sets the C flag if overflow
    auto CFlag = ((a + b) & 0x100) >> 0x04;
    AF.low_8 &= 0xEF;
    AF.low_8 |= CFlag;
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
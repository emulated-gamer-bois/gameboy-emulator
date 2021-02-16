//
// Created by davidm on 2021-02-12.
//

#include "cpu.h"

CPU::CPU(u_int16_t PC, std::shared_ptr<MMU> mmu) {
    this->PC = PC;
    this->memory = mmu;
    SP.all_16 = 0x00;
    AF.all_16 = 0x00;
    BC.all_16 = 0x00;
    DE.all_16 = 0x00;
    HL.all_16 = 0x00;
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

void loadIm16(uint8_t firstByte, uint8_t secondByte, RegisterPair &reg) {
    reg.low_8 =  firstByte;
    reg.high_8 = secondByte;
}

void loadIm8(uint8_t firstByte, uint8_t &reg) {
    reg = firstByte;
}

void CPU::loadImp(uint8_t &reg, uint16_t addr) {
    reg = memory->read(addr);
}

/**
 * Used for LD (BC), A or similar
 * @param reg (BC)
 * @param value value fo A
 */
void CPU::storeAddr(uint16_t addr, uint8_t value) {
    memory->write(addr, value);
}

//Logical operations ******************

/**
 * Executes AND with the A register and the given value
 * stores the result in A
 */
void CPU::andA(uint8_t value) {
    AF.high_8 &= value;
    setZNFlags(AF.high_8, false);

    //Sets H flag = 1, C = 0
    AF.low_8 &= 0xC0;
    AF.low_8 |= 0x20;
}

/**
 * Executes XOR with the A register and the given value
 * stores the result in A
 */
void CPU::xorA(uint8_t value) {
    AF.high_8 ^= value;
    setZNFlags(AF.high_8, false);

    //Sets all flags except Z to 0
    AF.low_8 &= 0x80;
}

/**
 * Executes OR with the A register and the given value
 * stores the result in A
 */
void CPU::orA(uint8_t value) {
    AF.high_8 |= value;
    setZNFlags(AF.high_8, false);

    //Sets all flags except Z to 0
    AF.low_8 &= 0x80;
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
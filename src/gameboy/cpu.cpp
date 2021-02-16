//
// Created by davidm on 2021-02-12.
//

#include "cpu.h"

CPU::CPU(u_int16_t PC, std::shared_ptr<MMU> mmu) {
    this->PC = PC;
    this->memory = mmu;
}

void nop() {}

void ldsp(u_int8_t first_byte, u_int8_t second_byte, RegisterPair &SP) {
    SP.low_8 = first_byte;
    SP.high_8 = second_byte;
}

void CPU::setZNFlags(uint8_t newValue, bool subtraction) {
    //Sets the Z flag to 1 if newValue == 0
    AF.low_8 &= 0x7F;
    AF.low_8 |= newValue == 0x00 ? 0x80 : 0x00;

    //Set the N flag if operation is subtraction
    AF.low_8 |= subtraction ? 0x40 : 0x00;
    AF.low_8 &= !subtraction ? 0xBF : 0xFF;
}

void CPU::setHFlag(uint8_t a, uint8_t b) {
    // Sets the H flag if carry from bit 3 to bit 4
    auto HFlag = (((a & 0x0F) + (b & 0x0F)) & 0x10) << 0x01;
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
    reg.low_8 = firstByte;
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

/**
 *
 * Only adding from A, as we know that A is the only register allowed to use for arithmetics.
 * Can do both with carry and without, depending on the
 * */
void CPU::addImp(uint8_t value, bool withCarry) {
    auto CFlag = withCarry ? ((AF.low_8 & 0x10) > 0x04) : 0;
    setCFlag(AF.high_8, value + CFlag);
    AF.high_8 += value + CFlag;
    setZNFlags(AF.high_8, false);
}

uint8_t twosComp(uint8_t value) {
    return ~value + 1;
}
/**
 * Uncertain if carry flag here is supposed to add or remove a value?
 * */
void CPU::subImp(uint8_t value, bool withCarry) {
    auto CFlag = withCarry ? ((AF.low_8 & 0x10) > 0x04) : 0;
    auto twosCompVal = twosComp(value);
    setCFlag(AF.high_8, value);
    AF.high_8 += twosCompVal + CFlag;
    setZNFlags(AF.high_8, true);
}
/**
 * Will have to consider the increment and decrement of 16 bit addresses where flags are to be set
 * later as well (HL).
 */
void increment16(uint16_t &addr){
    addr+=1;
}
void CPU::increment8(uint8_t &addr,bool setFlags){
    if(setFlags){
        setZNFlags(addr+1, false);
        setHFlag(addr,0x1);
    }
    addr+=1;
}
void CPU::decrement16(uint16_t &addr){
    addr-=1;
}

void CPU::decrement8(uint8_t &addr,bool setFlags){
    if(setFlags){
        setZNFlags(addr+1, true);
        setHFlag(addr,0x1);
    }
    addr-=1;
}

void CPU::execute_cycle() {
    switch (memory->read(PC++)) {
        case 0x00: //NOP
            nop();
            break;
        case 0x31: // LD SP, d16
            ldsp(memory->read(PC), memory->read(PC + 1), SP);
            PC += 2;
            break;

        default:
            nop();
            break;
    }
}
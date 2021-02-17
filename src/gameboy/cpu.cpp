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

void CPU::decrement8(uint8_t &addr,bool setFlags) {
    if (setFlags) {
        setZNFlags(addr + 1, true);
        setHFlag(addr, 0x1);
    }
    addr -= 1;
//Logical operations ******************
}
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

/**
 * Rotate left, d7 -> C flag and d7 -> d0
 */
void CPU::rlc(uint8_t &reg) {
    auto d7 = (reg & 0x80) >> 0x07;
    reg = (reg << 1) | d7;

    //Sets C flag to d7
    AF.low_8 = (AF.low_8 & 0xEF) | (d7 << 4);
}

/**
 * Rotate left, C -> d0 and d7 -> C
 */
 void CPU::rl(uint8_t &reg) {
    auto d7 = (reg & 0x80) >> 0x07;
    reg = (reg << 1) | ((AF.low_8 >> 4) & 0x01);

    //Sets C flag to d7
    AF.low_8 = (AF.low_8 & 0xEF) | (d7 << 4);
 }

/**
* Rotate right, d0 -> C flag and d0 -> d7
*/
void CPU::rrc(uint8_t &reg) {
    auto d0 = reg & 0x01;
    reg = (reg >> 1) | (d0 << 7);

    //Sets C flag to d0
    AF.low_8 = (AF.low_8 & 0xEF) | (d0 << 4);
}

/**
 * Rotate left, C -> d7 and d0 -> C
 */
void CPU::rr(uint8_t &reg) {
    auto d0 = reg & 0x01;
    reg = (reg >> 1) | ((AF.low_8 << 3) & 0x80);

    //Sets C flag to d7
    AF.low_8 = (AF.low_8 & 0xEF) | (d0 << 4);
}

void CPU::execute_cycle() {
    switch (memory->read(PC++)) {
        case 0x00: //NOP
            nop();
            break;
        case 0x07: //RLCA
            rlc(AF.high_8);
            break;
        case 0x0F: //RRCA
            rrc(AF.high_8);
            break;
        case 0x17: //RLA
            rl(AF.high_8);
            break;
        case 0x1F: //RRA
            rr(AF.high_8);
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
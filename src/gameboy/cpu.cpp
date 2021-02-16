//
// Created by davidm on 2021-02-12.
//

#include "cpu.h"
#include "mmu.h"

CPU::CPU(uint16_t PC, std::shared_ptr<MMU> mmu) {
    this->PC = PC;
    this->memory = mmu;
    SP.all_16 = 0x00;
    AF.all_16 = 0x00;
    BC.all_16 = 0x00;
    DE.all_16 = 0x00;
    HL.all_16 = 0x00;
}

void nop() {}

void ldsp(uint8_t first_byte, uint8_t second_byte, RegisterPair &SP) {
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
 * Executes ADD with the A register and the given value
 * stores the result in A. Can be done with or without carry.
 */
void CPU::addA(uint8_t value, bool withCarry) {
    auto CFlag = withCarry ? ((AF.low_8 & 0x10) > 0x04) : 0;
    setCFlag(AF.high_8, value + CFlag);
    setHFlag(AF.high_8,value+CFlag);
    AF.high_8 += value + CFlag;
    setZNFlags(AF.high_8, false);
}

uint8_t twosComp(uint8_t value) {
    return ~value + 1;
}

/**
 * Executes SUB with the A register and the given value
 * stores the result in A. Can be done with or without carry,carry
 * currently subtracts 1 if set.
 */
void CPU::subA(uint8_t value, bool withCarry) {
    auto CFlag = withCarry ? ((AF.low_8 & 0x10) > 0x04) : 0;
    auto twosCompVal = twosComp(value);
    setHFlag(AF.high_8, twosCompVal - CFlag);
    setCFlag(AF.high_8, value);
    AF.high_8 += twosCompVal - CFlag;
    setZNFlags(AF.high_8, true);
}

/**
 * Will have to consider the increment and decrement of 16 bit addresses where flags are to be set
 * later as well (HL).
 */
void increment16(uint16_t &addr) {
    addr += 1;
}

void CPU::increment8(uint8_t &addr) {

    setZNFlags(addr + 1, false);
    setHFlag(addr, 0x1);
    addr += 1;
}

void CPU::decrement16(uint16_t &addr) {
    addr -= 1;
}

void CPU::decrement8(uint8_t &addr) {
    setZNFlags(addr - 1, true);
    setHFlag(addr, twosComp(0x1));
    addr -= 1;
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
    switch (memory->read(this->PC++)) {
        case 0x00: //NOP
            nop();
            break;
        case 0x03:
            increment16(BC.all_16);
            break;
        case 0x04:
            increment8(BC.high_8);
            break;
        case 0x05:
            decrement8(BC.high_8);
            break;
        case 0x07: //RLCA
            rlc(AF.high_8);
            break;
        case 0x0B:
            decrement16(BC.all_16);
            break;
        case 0x0C:
            increment8(BC.low_8);
            break;
        case 0x0D:
            decrement8(BC.low_8);
            break;
        case 0x0F: //RRCA
            rrc(AF.high_8);
            break;
        case 0x13:
            increment16(DE.all_16);
            break;
        case 0x14:
            increment8(DE.high_8);
            break;
        case 0x15:
            decrement8(DE.high_8);
            break;
        case 0x17: //RLA
            rl(AF.high_8);
            break;
        case 0x1B:
            decrement16(DE.all_16);
            break;
        case 0x1C:
            increment8(DE.low_8);
            break;
        case 0x1D:
            decrement8(DE.low_8);
            break;
        case 0x1F: //RRA
            rr(AF.high_8);
            break;
        case 0x23:
            increment16(HL.all_16);
            break;
        case 0x24:
            increment8(HL.high_8);
            break;
        case 0x25:
            decrement8(HL.high_8);
            break;
        case 0x2B:
            decrement16(HL.all_16);
            break;
        case 0x2C:
            increment8(HL.low_8);
            break;
        case 0x2D:
            decrement8(HL.low_8);
            break;
        case 0x31: // LD SP, d16
            ldsp(memory->read(PC), memory->read(this->PC+1), this->SP);
            this->PC += 2;
            break;
        case 0x33:
            increment16(SP.all_16);
            break;
        case 0x3B:
            decrement16(SP.all_16);
            break;
        case 0x3C:
            increment8(AF.high_8);
            break;
        case 0x3D:
            decrement8(AF.high_8);
            break;
        case 0x80:
            addA(BC.high_8, false);
            break;
        case 0x81:
            addA(BC.low_8, false);
            break;
        case 0x82:
            addA(DE.high_8, false);
            break;
        case 0x83:
            addA(DE.low_8, false);
            break;
        case 0x84:
            addA(HL.high_8, false);
            break;
        case 0x85:
            addA(HL.low_8, false);
            break;

            //TODO missing 0x86

        case 0x87:
            addA(AF.high_8, false);
            break;
        case 0x88:
            addA(BC.high_8, true);
            break;
        case 0x89:
            addA(BC.low_8, true);
            break;
        case 0x8A:
            addA(DE.high_8, true);
            break;
        case 0x8B:
            addA(DE.low_8, true);
            break;
        case 0x8C:
            addA(HL.high_8, true);
            break;
        case 0x8D:
            addA(HL.low_8, true);
            break;
            //TODO missing x08E
        case 0x8F:
            addA(AF.high_8, true);
            break;
        case 0x90:
            subA(BC.high_8, false);
            break;
        case 0x91:
            subA(BC.low_8, false);
            break;
        case 0x92:
            subA(DE.high_8, false);
            break;
        case 0x93:
            subA(DE.low_8, false);
            break;
        case 0x94:
            subA(HL.high_8, false);
            break;
        case 0x95:
            subA(HL.low_8, false);
            break;
        case 0x96:
            subA(memory->read(HL.all_16), false);
            break;
        case 0x97:
            subA(AF.high_8, false);
            break;
        case 0x98:
            subA(BC.high_8, true);
            break;
        case 0x99:
            subA(BC.low_8, true);
            break;
        case 0x9A:
            subA(DE.high_8, true);
            break;
        case 0x9B:
            subA(DE.low_8, true);
            break;
        case 0x9C:
            subA(HL.high_8, true);
            break;
        case 0x9D:
            subA(HL.low_8, true);
            break;
            //TODO missing 0x9E
        case 0x9F:
            subA(AF.high_8, true);
            break;
        default:
            nop();
            break;
    }
}

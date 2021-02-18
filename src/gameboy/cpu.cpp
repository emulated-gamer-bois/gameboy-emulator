//
// Created by davidm on 2021-02-12.
//

#include "cpu.h"

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

void loadIm8(uint8_t &reg, uint8_t firstByte) {
    reg = firstByte;
}

void CPU::loadImp(uint16_t addr, uint8_t &reg) {
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
    setHFlag(AF.high_8, value + CFlag);
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

/**
 * Every time we read PC, we want to increment it.
 * */
uint16_t CPU::read_and_inc_pc() {
    return memory->read(PC++);
}
uint16_t combine_bytes(uint8_t first_byte, uint8_t second_byte) {
    //first byte low, second high.
    return second_byte << 8 | first_byte;
}

void CPU::execute_cycle() {
    switch (read_and_inc_pc()) {
        case 0x00:
            nop();
            break;
        case 0x01:
            loadIm16(read_and_inc_pc(), read_and_inc_pc(), BC);
            break;
        case 0x02:
            storeAddr(BC.all_16, AF.high_8);
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
        case 0x06:
            loadIm8(BC.high_8,read_and_inc_pc());
            break;
        case 0x07: //RLCA
            rlc(AF.high_8);
            break;
        case 0x08:
            //special case, not using read_and_inc PC
            storeAddr(combine_bytes(memory->read(PC), memory->read(PC + 1)), SP.low_8);
            storeAddr(combine_bytes(memory->read(PC), memory->read(PC + 1)) + 1, SP.high_8);
            PC += 2;
            break;
        case 0x0A:
            loadImp(BC.all_16, AF.high_8);
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
        case 0x0E:
            loadIm8(BC.low_8,read_and_inc_pc());
            break;
        case 0x0F: //RRCA
            rrc(AF.high_8);
            break;
        case 0x11:
            loadIm16(read_and_inc_pc(), read_and_inc_pc(), DE);
            break;
        case 0x12:
            storeAddr(DE.all_16, AF.high_8);
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
        case 0x16:
            loadIm8( DE.high_8,read_and_inc_pc());
            break;
        case 0x17: //RLA
            rl(AF.high_8);
            break;
        case 0x1A:
            loadImp(DE.all_16, AF.high_8);
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
        case 0x1E:
            loadIm8(DE.low_8,read_and_inc_pc());
            break;
        case 0x1F: //RRA
            rr(AF.high_8);
            break;
        case 0x21:
            loadIm16(read_and_inc_pc(), read_and_inc_pc(), HL);
            break;
        case 0x22:
            storeAddr(HL.all_16, AF.high_8);
            increment16(HL.all_16);
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
        case 0x26:
            loadIm8(HL.high_8,read_and_inc_pc());
            break;
        case 0x2A:
            loadImp(HL.all_16, AF.high_8);
            increment16(HL.all_16);
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
        case 0x2E:
            loadIm8(HL.low_8,read_and_inc_pc());
            break;
        case 0x31:
            loadIm16(memory->read(PC), memory->read(PC + 1), SP);
            PC += 2;
            break;
        case 0x32:
            storeAddr(HL.all_16, AF.high_8);
            decrement16(HL.all_16);
            break;
        case 0x33:
            increment16(SP.all_16);
            break;
            //TODO 0X34 0X35
        case 0x36:
            storeAddr(HL.all_16, read_and_inc_pc());
            break;
        case 0x37: //RLA
            rl(AF.high_8);
            break;
        case 0x3A:
            loadImp(HL.all_16, AF.high_8);
            decrement16(HL.all_16);
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
        case 0x3E:
            loadIm8(AF.high_8,read_and_inc_pc());
            break;
        case 0x40:
            //This is stoopid.
            loadIm8(BC.high_8,BC.high_8);
            break;
        case 0x41:
            loadIm8(BC.high_8,BC.low_8);
            break;
        case 0x42:
            loadIm8(BC.high_8,DE.high_8);
            break;
        case 0x43:
            loadIm8(BC.high_8,DE.low_8);
            break;
        case 0x44:
            loadIm8(BC.high_8,HL.high_8);
            break;
        case 0x45:
            loadIm8(BC.high_8,HL.low_8);
            break;
        case 0x46:
            loadIm8(BC.high_8,memory->read(HL.all_16));
            break;
        case 0x47:
            loadIm8(BC.high_8,AF.high_8);
            break;
        case 0x48:
            loadIm8(BC.low_8,BC.high_8);
            break;
        case 0x49:
            loadIm8(BC.low_8,BC.low_8);
            break;
        case 0x4A:
            loadIm8(BC.low_8,DE.high_8);
            break;
        case 0x4B:
            loadIm8(BC.low_8,DE.low_8);
            break;
        case 0x4C:
            loadIm8(BC.low_8,HL.high_8);
            break;
        case 0x4D:
            loadIm8(BC.low_8,HL.low_8);
            break;
        case 0x4E:
            loadIm8(BC.low_8,memory->read(HL.all_16));
            break;
        case 0x4F:
            loadIm8(BC.low_8,AF.high_8);
            break;
        case 0x50:
            loadIm8(DE.high_8,BC.high_8);
            break;
        case 0x51:
            loadIm8(DE.high_8,BC.low_8);
            break;
        case 0x52:
            loadIm8(DE.high_8,DE.high_8);
            break;
        case 0x53:
            loadIm8(DE.high_8,DE.low_8);
            break;
        case 0x54:
            loadIm8(DE.high_8,HL.high_8);
            break;
        case 0x55:
            loadIm8(DE.high_8,HL.low_8);
            break;
        case 0x56:
            loadIm8(DE.high_8,memory->read(HL.all_16));
            break;
        case 0x57:
            loadIm8(DE.high_8,AF.high_8);
            break;
        case 0x58:
            loadIm8(DE.low_8,BC.high_8);
            break;
        case 0x59:
            loadIm8(DE.low_8,BC.low_8);
            break;
        case 0x5A:
            loadIm8(DE.low_8,DE.high_8);
            break;
        case 0x5B:
            loadIm8(DE.low_8,DE.low_8);
            break;
        case 0x5C:
            loadIm8(DE.low_8,HL.high_8);
            break;
        case 0x5D:
            loadIm8(DE.low_8,HL.low_8);
            break;
        case 0x5E:
            loadIm8(DE.low_8,memory->read(HL.all_16));
            break;
        case 0x5F:
            loadIm8(BC.low_8,AF.high_8);
            break;
        case 0x60:
            loadIm8(HL.high_8,BC.high_8);
            break;
        case 0x61:
            loadIm8(HL.high_8,BC.low_8);
            break;
        case 0x62:
            loadIm8(HL.high_8,DE.high_8);
            break;
        case 0x63:
            loadIm8(HL.high_8,DE.low_8);
            break;
        case 0x64:
            loadIm8(HL.high_8,HL.high_8);
            break;
        case 0x65:
            loadIm8(HL.high_8,HL.low_8);
            break;
        case 0x66:
            loadIm8(HL.high_8,memory->read(HL.all_16));
            break;
        case 0x67:
            loadIm8(HL.high_8,AF.high_8);
            break;
        case 0x68:
            loadIm8(HL.low_8,BC.high_8);
            break;
        case 0x69:
            loadIm8(HL.low_8,BC.low_8);
            break;
        case 0x6A:
            loadIm8(HL.low_8,DE.high_8);
            break;
        case 0x6B:
            loadIm8(HL.low_8,DE.low_8);
            break;
        case 0x6C:
            loadIm8(HL.low_8,HL.high_8);
            break;
        case 0x6D:
            loadIm8(HL.low_8,HL.low_8);
            break;
        case 0x6E:
            loadIm8(HL.low_8,memory->read(HL.all_16));
            break;
        case 0x6F:
            loadIm8(HL.low_8,AF.high_8);
            break;
        case 0x70:
            storeAddr(HL.all_16,BC.high_8);
            break;
        case 0x71:
            storeAddr(HL.all_16,BC.low_8);
            break;
        case 0x72:
            storeAddr(HL.all_16,DE.high_8);
            break;
        case 0x73:
            storeAddr(HL.all_16,DE.low_8);
            break;
        case 0x74:
            storeAddr(HL.all_16,HL.high_8);
            break;
        case 0x75:
            storeAddr(HL.all_16,HL.low_8);
            break;
        case 0x77:
            storeAddr(HL.all_16,AF.high_8);
            break;
        case 0x78:
            loadIm8(AF.high_8,BC.high_8);
            break;
        case 0x79:
            loadIm8(AF.high_8,BC.low_8);
            break;
        case 0x7A:
            loadIm8(AF.high_8,DE.high_8);
            break;
        case 0x7B:
            loadIm8(AF.high_8,DE.low_8);
            break;
        case 0x7C:
            loadIm8(AF.high_8,HL.high_8);
            break;
        case 0x7D:
            loadIm8(AF.high_8,HL.low_8);
            break;
        case 0x7E:
            loadIm8(AF.high_8,memory->read(HL.all_16));
            break;
        case 0x7F:
            loadIm8(AF.high_8,AF.high_8);
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
        case 0x86:
            addA(memory->read(HL.all_16), false);
            break;
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
        case 0x8E:
            addA(memory->read(HL.all_16), true);
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
        case 0x9E:
            subA(memory->read(HL.all_16), true);
            break;
        case 0x9F:
            subA(AF.high_8, true);
            break;
        default:
            nop();
            break;
    }
}



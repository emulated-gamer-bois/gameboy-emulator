//
// Created by davidm on 2021-02-12.
//

#include "cpu.h"
#include "mmu.h"

uint16_t combine_bytes(uint8_t first_byte, uint8_t second_byte);

CPU::CPU(uint16_t PC, uint16_t SP, std::shared_ptr<MMU> mmu) {
    this->PC = PC;
    this->memory = mmu;
    this->SP.all_16 = SP;
    AF.all_16 = 0x00;
    BC.all_16 = 0x00;
    DE.all_16 = 0x00;
    HL.all_16 = 0x00;
}

void nop() {}

/**
 * Z set if newValue is 0
 * N set if subtraction is true
 */
void CPU::setZNFlags(uint8_t newValue, bool subtraction) {
    //Sets the Z flag to 1 if newValue == 0
    AF.low_8 &= 0x7F;
    AF.low_8 |= newValue == 0x00 ? 0x80 : 0x00;

    //Set the N flag if operation is subtraction
    AF.low_8 &= !subtraction ? 0xBF : 0xFF;
    AF.low_8 |= subtraction ? 0x40 : 0x00;
}

/**
 * Sets H flag if there is a carry between bit 3 and bit 4
 * @param a parameter 1 in addition
 * @param b parameter 2 in addition
 */
void CPU::setHFlag(uint8_t a, uint8_t b) {
    // Sets the H flag if carry from bit 3 to bit 4
    auto HFlag = (((a & 0x0F) + (b & 0x0F)) & 0x10) << 0x01;
    AF.low_8 &= 0xDF;
    AF.low_8 |= HFlag;
}

/**
 * Sets C flag if there is a carry between bit 7 and bit 8
 * @param a parameter 1 in addition
 * @param b parameter 2 in addition
 */
void CPU::setCFlag(uint8_t a, uint8_t b) {
    // Sets the C flag if overflow
    auto CFlag = (((uint16_t) a + b) & 0x100) >> 0x04;
    AF.low_8 &= 0xEF;
    AF.low_8 |= CFlag;
}

/**
 * Loads the immediate value of the two bytes into the provided register
 * @param firstByte the first argument to the operation code, will be loaded to the lower part of the register
 * @param secondByte the second argument to the operation code, will be loaded to the higher part of the register
 */
void CPU::loadIm16(uint8_t firstByte, uint8_t secondByte, RegisterPair &reg) {
    reg.low_8 = firstByte;
    reg.high_8 = secondByte;
}

/**
 * Loads the immediate value of the value of the op-code parameter
 */
void CPU::loadIm8(uint8_t &reg, uint8_t firstByte) {
    reg = firstByte;
}

/**
 * Loads the specified register with the value found on the specified address
 */
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
    auto CFlag = withCarry ? ((AF.low_8 & 0x10) >> 0x04) : 0;
    setCFlag(AF.high_8, value + CFlag);
    setHFlag(AF.high_8, value + CFlag);
    AF.high_8 += value + CFlag;
    setZNFlags(AF.high_8, false);
}

/**
 * Makes a number negative by converting it to two complement
 */
uint8_t twosComp(uint8_t value) {
    return ~value + 1;
}

/**
 * Executes SUB with the A register and the given value
 * stores the result in A. Can be done with or without carry,carry
 * currently subtracts 1 if set.
 */
void CPU::subA(uint8_t value, bool withCarry) {
    auto CFlag = withCarry ? ((AF.low_8 & 0x10) >> 0x04) : 0;
    value = twosComp(value + CFlag);
    setCFlag(AF.high_8, value);
    setHFlag(AF.high_8, value);
    AF.high_8 += value;
    setZNFlags(AF.high_8, true);
}

/**
 * Will have to consider the increment and decrement of 16 bit addresses where flags are to be set
 * later as well (HL).
 */
void CPU::increment16(uint16_t &reg) {
    reg += 1;
}

/**
 * Increment the value of the specified 8 bit register with one
 */
void CPU::increment8(uint8_t &reg) {
    setZNFlags(reg + 1, false);
    setHFlag(reg, 0x1);
    reg += 1;
}

/**
 * Decrement the value of the specified 16 bit register with one
 */
void CPU::decrement16(uint16_t &reg) {
    reg -= 1;
}

/**
 * Decrement the value of the specified 8 bit register with one
 */
void CPU::decrement8(uint8_t &addr) {
    setZNFlags(addr - 1, true);
    setHFlag(addr, twosComp(0x1));
    addr -= 1;
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

/**
 * Executes subtraction between value and register A and sets flags accordingly
 * the result is not saved
 */
void CPU::compareA(uint8_t value) {
    value = twosComp(value);
    setCFlag(AF.high_8, value);
    setHFlag(AF.high_8, value);
    setZNFlags(AF.high_8 + value, true);
}


/**
 * Stores the value of the given reg on the stack
 * SP is adjusted accordingly
 */
void CPU::pushReg(RegisterPair &reg) {
    memory->write(--SP.all_16, reg.high_8);
    memory->write(--SP.all_16, reg.low_8);
}

/**
 * Stores the first two bytes from the stack in the given reg
 * SP is adjusted accordingly
 */
void CPU::popReg(RegisterPair &reg) {
    reg.low_8 = memory->read(SP.all_16++);
    reg.high_8 = memory->read(SP.all_16++);
}

/**
 * Jumps immediately to the specified address
 */
void CPU::jump(uint16_t addr) {
    PC = addr;
}

/**
 * Jumps immediately to the specified address if Z is one or zero
 * depending on the if_one parameter
 */
void CPU::jumpZ(uint16_t addr, bool if_one) {
    if(!if_one  ==  !(AF.low_8 & 0x80)) PC = addr;
}

/**
 * Jumps immediately to the specified address if C is one or zero
 * depending on the if_one parameter
 */
void CPU::jumpC(uint16_t addr, bool if_one) {
    if(!if_one  ==  !(AF.low_8 & 0x10)) PC = addr;
}

/**
 * Increments PC with the given number of steps
 */
void CPU::jumpRelative(int8_t steps) {
    PC += steps;
}

/**
 * Increments PC with the given number of steps if Z is one or zero
 * depending on the if_one parameter
 */
void CPU::jumpRelativeZ(int8_t steps, bool if_one) {
    if(!if_one  ==  !(AF.low_8 & 0x80)) PC += steps;
}

/**
 * Increments PC with the given number of steps if C is one or zero
 * depending on the if_one parameter
 */
void CPU::jumpRelativeC(int8_t steps, bool if_one) {
    if(!if_one  ==  !(AF.low_8 & 0x10)) PC += steps;
}

/**
 * Call sub routine
 * Stores value of PC on the stack and sets the value of PC to the immediate data
 * @param firstByte fist parameter and the lower byte
 * @param secondByte second parameter the higher byte
 */
void CPU::call(uint8_t firstByte, uint8_t secondByte) {
    memory->write(--SP.all_16, PC >> 0x08);
    memory->write(--SP.all_16, PC & 0x00FF);
    PC = combine_bytes(firstByte, secondByte);
}

/**
 * Calls subroutine if Z is one or zero
 * depending on the if_one parameter
 * @param firstByte fist parameter and the lower byte
 * @param secondByte second parameter the higher byte
 * @param if_one true if Z should be 1, false if Z should be 0
 */
void CPU::callZ(uint8_t firstByte, uint8_t secondByte, bool if_one) {
    if(!if_one  !=  !(AF.low_8 & 0x80)) return;
    call(firstByte, secondByte);
}

/**
 * Calls subroutine if C is one or zero
 * depending on the if_one parameter
 * @param firstByte fist parameter and the lower byte
 * @param secondByte second parameter the higher byte
 * @param if_one true if C should be 1, false if C should be 0
 */
void CPU::callC(uint8_t firstByte, uint8_t secondByte, bool if_one) {
    if(!if_one  !=  !(AF.low_8 & 0x10)) return;
    call(firstByte, secondByte);
}

/**
 * Return from a subroutine by setting PC with
 * the two latest values on the stack
 * @param from_interrupt if the subroutine returns from interrupt
 */
void CPU::ret(bool from_interrupt) {
    PC = memory->read(SP.all_16++);
    PC |= memory->read(SP.all_16++) << 0x08;
    if(from_interrupt) {
        //TODO: Reset the interrupt flag
    }
}

/**
 * Return from subroutine if Z is one or zero
 * depending on the if_one parameter
 * @param if_one true if Z should be 1, false if Z should be 0
 */
void CPU::retZ(bool if_one) {
    if(!if_one  !=  !(AF.low_8 & 0x80)) return;
    ret(false);
}

/**
 * Return from subroutine if C is one or zero
 * depending on the if_one parameter
 * @param if_one true if C should be 1, false if C should be 0
 */
void CPU::retC(bool if_one) {
    if(!if_one  !=  !(AF.low_8 & 0x10)) return;
    ret(false);
}

/**
 * Restart PC from the nth byte (n as in number)
 * pushes PC to the stack before resetting PC
 * @param nth_byte vale from 0 to 7 of how many bytes from 0 PC should start at
 */
void CPU::reset(uint8_t nth_byte) {
    call(0x08 * nth_byte, 0x00);
}

/**
 * Sets the Z flag to the complement of bit number bit_no from value
 * @param bit_no bit 0 to 7
 * @param value the value of the byte which should be used
 */
void CPU::bit(uint8_t bit_nr, uint8_t value) {
    auto newZ = (~value >> bit_nr) & 0x01;
    AF.low_8 &= 0x70;
    AF.low_8 |= newZ << 0x07;

    //Sets N and H flag according to documentation
    AF.low_8 &= 0x90;
    AF.low_8 |= 0x20;
}

/**
 * Every time we read PC, we want to increment it.
 * */
uint8_t CPU::read_and_inc_pc() {
    return memory->read(PC++);
}

/**
 * Reads the two upcoming bytes, returns their combined value and incs PC twice
 * The first byte is the lower byte
 * The second byte is the higher byte
 * @return
 */
uint16_t CPU::read16_and_inc_pc() {
    PC += 1;
    return combine_bytes(memory->read(PC-2), memory->read(PC-1));
}

/**
 * /**
 * Combines two bytes to a 16 bit variable
 * @param first_byte 8 lowest bits
 * @param second_byte 8 highest bits
 */
uint16_t combine_bytes(uint8_t first_byte, uint8_t second_byte) {
    return second_byte << 8 | first_byte;
}

/**
 * Fetches, decodes and executes the instruction at location PC
 */
void CPU::execute_instruction() {
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
        case 0x18:
            jumpRelative(read_and_inc_pc());
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
        case 0x20:
            jumpRelativeZ(read_and_inc_pc(), false);
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
        case 0x28:
            jumpRelativeZ(read_and_inc_pc(), true);
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
        case 0x30:
            jumpRelativeC(read_and_inc_pc(), false);
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
        case 0x38:
            jumpRelativeC(read_and_inc_pc(), true);
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
        case 0xA0:
            andA(BC.high_8);
            break;
        case 0xA1:
            andA(BC.low_8);
            break;
        case 0xA2:
            andA(DE.high_8);
            break;
        case 0xA3:
            andA(DE.low_8);
            break;
        case 0xA4:
            andA(HL.high_8);
            break;
        case 0xA5:
            andA(HL.low_8);
            break;
        case 0xA6:
            andA(memory->read(HL.all_16));
            break;
        case 0xA7:
            andA(AF.high_8);
            break;
        case 0xA8:
            xorA(BC.high_8);
            break;
        case 0xA9:
            xorA(BC.low_8);
            break;
        case 0xAA:
            xorA(DE.high_8);
            break;
        case 0xAB:
            xorA(DE.low_8);
            break;
        case 0xAC:
            xorA(HL.high_8);
            break;
        case 0xAD:
            xorA(HL.low_8);
            break;
        case 0xAE:
            xorA(memory->read(HL.all_16));
            break;
        case 0xAF:
            xorA(AF.high_8);
            break;
        case 0xB0:
            orA(BC.high_8);
            break;
        case 0xB1:
            orA(BC.low_8);
            break;
        case 0xB2:
            orA(DE.high_8);
            break;
        case 0xB3:
            orA(DE.low_8);
            break;
        case 0xB4:
            orA(HL.high_8);
            break;
        case 0xB5:
            orA(HL.low_8);
            break;
        case 0xB6:
            orA(memory->read(HL.all_16));
            break;
        case 0xB7:
            orA(AF.high_8);
            break;
        case 0xB8:
            compareA(BC.high_8);
            break;
        case 0xB9:
            compareA(BC.low_8);
            break;
        case 0xBA:
            compareA(DE.high_8);
            break;
        case 0xBB:
            compareA(DE.low_8);
            break;
        case 0xBC:
            compareA(HL.high_8);
            break;
        case 0xBD:
            compareA(HL.low_8);
            break;
        case 0xBE:
            compareA(memory->read(HL.all_16));
            break;
        case 0xBF:
            compareA(AF.high_8);
            break;
        case 0xC0:
            retZ(false);
            break;
        case 0xC1:
            popReg(BC);
            break;
        case 0xC2:
            jumpZ(read16_and_inc_pc(), false);
            break;
        case 0xC3:
            jump(read16_and_inc_pc());
            break;
        case 0xC4:
            callZ(memory->read(PC), memory->read(PC+1), false);
            PC += 2;
            break;
        case 0xC5:
            pushReg(BC);
            break;
        case 0xC6:
            addA(read_and_inc_pc(),false);
            break;
        case 0xC7:
            reset(0);
            break;
        case 0xC8:
            retZ(true);
            break;
        case 0xC9:
            ret(false);
            break;
        case 0xCA:
            jumpZ(read16_and_inc_pc(), true);
            break;
        case 0xCC:
            callZ(memory->read(PC), memory->read(PC+1), true);
            PC += 2;
            break;
        case 0xCD:
            call(memory->read(PC), memory->read(PC+1));
            PC += 2;
            break;
        case 0xCE:
            addA(read_and_inc_pc(),true);
            break;
        case 0xCF:
            reset(1);
            break;
        case 0xD0:
            retC(false);
            break;
        case 0xD1:
            popReg(DE);
            break;
        case 0xD2:
            jumpC(read16_and_inc_pc(), false);
            break;
        case 0xD4:
            callC(memory->read(PC), memory->read(PC+1), false);
            PC += 2;
            break;
        case 0xD5:
            pushReg(DE);
            break;
        case 0xD6:
            subA(read_and_inc_pc(),false);
            break;
        case 0xD7:
            reset(2);
            break;
        case 0xD8:
            retC(true);
            break;
        case 0xD9:
            ret(true);
            break;
        case 0xDA:
            jumpC(read16_and_inc_pc(), true);
            break;
        case 0xDC:
            callC(memory->read(PC), memory->read(PC+1), true);
            PC += 2;
            break;
        case 0xDE:
            subA(read_and_inc_pc(),true);
            break;
        case 0xDF:
            reset(3);
            break;
        case 0xE1:
            popReg(HL);
            break;
        case 0xE5:
            pushReg(HL);
            break;
        case 0xE6:
            andA(read_and_inc_pc());
            break;
        case 0xE7:
            reset(4);
            break;
        case 0xE9:
            jump(HL.all_16);
            break;
        case 0xEE:
            xorA(read_and_inc_pc());
            break;
        case 0xEF:
            reset(5);
            break;
        case 0xF1:
            popReg(AF);
            break;
        case 0xF5:
            pushReg(AF);
            break;
        case 0xF6:
            orA(read_and_inc_pc());
            break;
        case 0xF7:
            reset(6);
            break;
        case 0xFE:
            compareA(read_and_inc_pc());
            break;
        case 0xFF:
            reset(7);
        default:
            nop();
            break;
    }
}

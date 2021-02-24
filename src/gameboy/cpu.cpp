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
    A.all_16 = 0x00;
    BC.all_16 = 0x00;
    DE.all_16 = 0x00;
    HL.all_16 = 0x00;
    F.all_8 = 0x0;
}

void nop() {}

/**
 * Z set if newValue is 0
 * N set if subtraction is true
 */
void CPU::setZNFlags(uint8_t newValue, bool subtraction) {
    //Sets the Z flag to 1 if newValue == 0
    F.z = newValue == 0 ? 1 : 0;
    //Set the N flag if operation is subtraction
    F.n = subtraction ? 1 : 0;

}

/**
 * Sets H flag if there is a carry between bit 3 and bit 4
 * @param a parameter 1 in addition
 * @param b parameter 2 in addition
 */
void CPU::setHFlag(uint8_t a, uint8_t b) {
    // Sets the H flag if carry from bit 3 to bit 4

    F.h = (((a & 0x0F) + (b & 0x0F)) & 0x10) == 0x10 ? 1 : 0;


}

/**
 * Sets C flag if there is a carry between bit 7 and bit 8
 * @param a parameter 1 in addition
 * @param b parameter 2 in addition
 */
void CPU::setCFlag(uint16_t a, uint16_t b) {
    // Sets the C flag if overflow
    F.c = (a + b) > 0xFF ? 1 : 0;
}

/**
 * Loads the immediate value of the two bytes into the provided register
 * @param firstByte the first argument to the operation code, will be loaded to the lower part of the register
 * @param secondByte the second argument to the operation code, will be loaded to the higher part of the register
 */
void CPU::loadIm16(uint16_t value ,RegisterPair &reg) {
    reg.all_16 = value;
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
    auto CFlag = withCarry ? F.c : 0;
    setCFlag(A.high_8, value + CFlag);
    setHFlag(A.high_8, value + CFlag);
    A.high_8 += value + CFlag;
    setZNFlags(A.high_8, false);
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
    auto CFlag = withCarry ? F.c : 0;
    value = twosComp(value + CFlag);
    setCFlag(A.high_8, value);
    setHFlag(A.high_8, value);
    A.high_8 += value;
    setZNFlags(A.high_8, true);
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
    A.high_8 &= value;
    setZNFlags(A.high_8, false);

    //Sets H flag = 1, C = 0
    F.h = 1;
    F.c = 0;

}

/**
 * Executes XOR with the A register and the given value
 * stores the result in A
 */
void CPU::xorA(uint8_t value) {
    A.high_8 ^= value;
    setZNFlags(A.high_8, false);

    //Sets all flags except Z to 0
    F.c = 0;
    F.n = 0;
    F.h = 0;
}

/**
 * Executes OR with the A register and the given value
 * stores the result in A
 */
void CPU::orA(uint8_t value) {
    A.high_8 |= value;
    setZNFlags(A.high_8, false);

    //Sets all flags except Z to 0
    F.c = 0;
    F.n = 0;
    F.h = 0;
}

/**
 * Rotate left, d7 -> C flag and d7 -> d0
 */
void CPU::rlc(uint8_t &reg) {
    auto d7 = (reg & 0x80) >> 0x07;
    reg = (reg << 1) | d7;

    //Sets C flag to d7
    F.c = d7 == 0 ? 0 : 1;
}

/**
 * Rotate left, C -> d0 and d7 -> C
 */
void CPU::rl(uint8_t &reg) {
    auto d7 = (reg & 0x80) >> 0x07;
    reg = (reg << 1) | ((F.all_8 >> 4) & 0x01);

    //Sets C flag to d7
    F.c = d7 == 0 ? 0 : 1;
}

/**
* Rotate right, d0 -> C flag and d0 -> d7
*/
void CPU::rrc(uint8_t &reg) {
    auto d0 = reg & 0x01;
    reg = (reg >> 1) | (d0 << 7);

    //Sets C flag to d0
    F.c = d0 == 0 ? 0 : 1;
}

/**
 * Rotate left, C -> d7 and d0 -> C
 */
void CPU::rr(uint8_t &reg) {
    auto d0 = reg & 0x01;
    auto d7 = reg & 0x80;
    reg = (reg >> 1) | ((F.all_8 << 3) & 0x80);

    //Sets C flag to d7
    F.c = d7 == 0 ? 0 : 1;
//    AF.low_8 = (AF.low_8 & 0xEF) | (d0 << 4);
}

/**
 * Executes subtraction between value and register A and sets flags accordingly
 * the result is not saved
 */
void CPU::compareA(uint8_t value) {
    value = twosComp(value);
    setCFlag(A.high_8, value);
    setHFlag(A.high_8, value);
    setZNFlags(A.high_8 + value, true);
}


/**
 * Stores the value of the given all_8 on the stack
 * SP is adjusted accordingly
 */
void CPU::pushReg(RegisterPair &reg) {
    memory->write(--SP.all_16, reg.high_8);
    memory->write(--SP.all_16, reg.low_8);
}

/**
 * Stores the first two bytes from the stack in the given all_8
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
    if (!if_one == !(F.z)) PC = addr;
}

/**
 * Jumps immediately to the specified address if C is one or zero
 * depending on the if_one parameter
 */
void CPU::jumpC(uint16_t addr, bool if_one) {
    if (!if_one == !(F.c)) PC = addr;
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
    if (!if_one == !(F.z)) PC += steps;
}

/**
 * Increments PC with the given number of steps if C is one or zero
 * depending on the if_one parameter
 */
void CPU::jumpRelativeC(int8_t steps, bool if_one) {
    if (!if_one == !(F.c)) PC += steps;
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
    if (!if_one != !(F.z)) return;
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
    if (!if_one != !(F.c)) return;
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
    if (from_interrupt) {
        //TODO: Reset the interrupt flag
    }
}

/**
 * Return from subroutine if Z is one or zero
 * depending on the if_one parameter
 * @param if_one true if Z should be 1, false if Z should be 0
 */
void CPU::retZ(bool if_one) {
    if (!if_one != !(F.z)) return;
    ret(false);
}

/**
 * Return from subroutine if C is one or zero
 * depending on the if_one parameter
 * @param if_one true if C should be 1, false if C should be 0
 */
void CPU::retC(bool if_one) {
    if (!if_one != !(F.c)) return;
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
    F.z = (~value >> bit_nr) & 0x01;
    F.n = 0;
    F.h = 1;
}
/**
 * Reset bit_nr in reg.
 * * */
void CPU::res(uint8_t bit_nr, uint8_t &reg) {
    reg = reg & (~0x01 << bit_nr);

}

/**
 * Set bit_nr in reg.
 * * */
void CPU::set(uint8_t bit_nr,uint8_t &reg){
    reg = reg | (0x01 << bit_nr);
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
    return combine_bytes(memory->read(PC - 2), memory->read(PC - 1));
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
            loadIm16(read16_and_inc_pc(),BC);


            break;
        case 0x02:
            storeAddr(BC.all_16, A.high_8);
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
            loadIm8(BC.high_8, read_and_inc_pc());
            break;
        case 0x07: //RLCA
            rlc(A.high_8);
            break;
        case 0x08:
            //special case, not using read_and_inc PC
            storeAddr(combine_bytes(memory->read(PC), memory->read(PC + 1)), SP.low_8);
            storeAddr(combine_bytes(memory->read(PC), memory->read(PC + 1)) + 1, SP.high_8);
            PC += 2;
            break;
        case 0x0A:
            loadImp(BC.all_16, A.high_8);
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
            loadIm8(BC.low_8, read_and_inc_pc());
            break;
        case 0x0F: //RRCA
            rrc(A.high_8);
            break;
        case 0x11:
            loadIm16(read16_and_inc_pc(), DE);
            break;
        case 0x12:
            storeAddr(DE.all_16, A.high_8);
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
            loadIm8(DE.high_8, read_and_inc_pc());
            break;
        case 0x17: //RLA
            rl(A.high_8);
            break;
        case 0x18:
            jumpRelative(read_and_inc_pc());
            break;
        case 0x1A:
            loadImp(DE.all_16, A.high_8);
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
            loadIm8(DE.low_8, read_and_inc_pc());
            break;
        case 0x1F: //RRA
            rr(A.high_8);
            break;
        case 0x20:
            jumpRelativeZ(read_and_inc_pc(), false);
            break;
        case 0x21:
            loadIm16(read16_and_inc_pc(), HL);
            break;
        case 0x22:
            storeAddr(HL.all_16, A.high_8);
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
            loadIm8(HL.high_8, read_and_inc_pc());
            break;
        case 0x28:
            jumpRelativeZ(read_and_inc_pc(), true);
            break;
        case 0x2A:
            loadImp(HL.all_16, A.high_8);
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
            loadIm8(HL.low_8, read_and_inc_pc());
            break;
        case 0x30:
            jumpRelativeC(read_and_inc_pc(), false);
            break;
        case 0x31:
            loadIm16(read16_and_inc_pc(), SP);
            PC += 2;
            break;
        case 0x32:
            storeAddr(HL.all_16, A.high_8);
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
            rl(A.high_8);
            break;
        case 0x38:
            jumpRelativeC(read_and_inc_pc(), true);
        case 0x3A:
            loadImp(HL.all_16, A.high_8);
            decrement16(HL.all_16);
            break;
        case 0x3B:
            decrement16(SP.all_16);
            break;
        case 0x3C:
            increment8(A.high_8);
            break;
        case 0x3D:
            decrement8(A.high_8);
            break;
        case 0x3E:
            loadIm8(A.high_8, read_and_inc_pc());
            break;
        case 0x40:
            //This is stoopid.
            loadIm8(BC.high_8, BC.high_8);
            break;
        case 0x41:
            loadIm8(BC.high_8, BC.low_8);
            break;
        case 0x42:
            loadIm8(BC.high_8, DE.high_8);
            break;
        case 0x43:
            loadIm8(BC.high_8, DE.low_8);
            break;
        case 0x44:
            loadIm8(BC.high_8, HL.high_8);
            break;
        case 0x45:
            loadIm8(BC.high_8, HL.low_8);
            break;
        case 0x46:
            loadIm8(BC.high_8, memory->read(HL.all_16));
            break;
        case 0x47:
            loadIm8(BC.high_8, A.high_8);
            break;
        case 0x48:
            loadIm8(BC.low_8, BC.high_8);
            break;
        case 0x49:
            loadIm8(BC.low_8, BC.low_8);
            break;
        case 0x4A:
            loadIm8(BC.low_8, DE.high_8);
            break;
        case 0x4B:
            loadIm8(BC.low_8, DE.low_8);
            break;
        case 0x4C:
            loadIm8(BC.low_8, HL.high_8);
            break;
        case 0x4D:
            loadIm8(BC.low_8, HL.low_8);
            break;
        case 0x4E:
            loadIm8(BC.low_8, memory->read(HL.all_16));
            break;
        case 0x4F:
            loadIm8(BC.low_8, A.high_8);
            break;
        case 0x50:
            loadIm8(DE.high_8, BC.high_8);
            break;
        case 0x51:
            loadIm8(DE.high_8, BC.low_8);
            break;
        case 0x52:
            loadIm8(DE.high_8, DE.high_8);
            break;
        case 0x53:
            loadIm8(DE.high_8, DE.low_8);
            break;
        case 0x54:
            loadIm8(DE.high_8, HL.high_8);
            break;
        case 0x55:
            loadIm8(DE.high_8, HL.low_8);
            break;
        case 0x56:
            loadIm8(DE.high_8, memory->read(HL.all_16));
            break;
        case 0x57:
            loadIm8(DE.high_8, A.high_8);
            break;
        case 0x58:
            loadIm8(DE.low_8, BC.high_8);
            break;
        case 0x59:
            loadIm8(DE.low_8, BC.low_8);
            break;
        case 0x5A:
            loadIm8(DE.low_8, DE.high_8);
            break;
        case 0x5B:
            loadIm8(DE.low_8, DE.low_8);
            break;
        case 0x5C:
            loadIm8(DE.low_8, HL.high_8);
            break;
        case 0x5D:
            loadIm8(DE.low_8, HL.low_8);
            break;
        case 0x5E:
            loadIm8(DE.low_8, memory->read(HL.all_16));
            break;
        case 0x5F:
            loadIm8(BC.low_8, A.high_8);
            break;
        case 0x60:
            loadIm8(HL.high_8, BC.high_8);
            break;
        case 0x61:
            loadIm8(HL.high_8, BC.low_8);
            break;
        case 0x62:
            loadIm8(HL.high_8, DE.high_8);
            break;
        case 0x63:
            loadIm8(HL.high_8, DE.low_8);
            break;
        case 0x64:
            loadIm8(HL.high_8, HL.high_8);
            break;
        case 0x65:
            loadIm8(HL.high_8, HL.low_8);
            break;
        case 0x66:
            loadIm8(HL.high_8, memory->read(HL.all_16));
            break;
        case 0x67:
            loadIm8(HL.high_8, A.high_8);
            break;
        case 0x68:
            loadIm8(HL.low_8, BC.high_8);
            break;
        case 0x69:
            loadIm8(HL.low_8, BC.low_8);
            break;
        case 0x6A:
            loadIm8(HL.low_8, DE.high_8);
            break;
        case 0x6B:
            loadIm8(HL.low_8, DE.low_8);
            break;
        case 0x6C:
            loadIm8(HL.low_8, HL.high_8);
            break;
        case 0x6D:
            loadIm8(HL.low_8, HL.low_8);
            break;
        case 0x6E:
            loadIm8(HL.low_8, memory->read(HL.all_16));
            break;
        case 0x6F:
            loadIm8(HL.low_8, A.high_8);
            break;
        case 0x70:
            storeAddr(HL.all_16, BC.high_8);
            break;
        case 0x71:
            storeAddr(HL.all_16, BC.low_8);
            break;
        case 0x72:
            storeAddr(HL.all_16, DE.high_8);
            break;
        case 0x73:
            storeAddr(HL.all_16, DE.low_8);
            break;
        case 0x74:
            storeAddr(HL.all_16, HL.high_8);
            break;
        case 0x75:
            storeAddr(HL.all_16, HL.low_8);
            break;
        case 0x77:
            storeAddr(HL.all_16, A.high_8);
            break;
        case 0x78:
            loadIm8(A.high_8, BC.high_8);
            break;
        case 0x79:
            loadIm8(A.high_8, BC.low_8);
            break;
        case 0x7A:
            loadIm8(A.high_8, DE.high_8);
            break;
        case 0x7B:
            loadIm8(A.high_8, DE.low_8);
            break;
        case 0x7C:
            loadIm8(A.high_8, HL.high_8);
            break;
        case 0x7D:
            loadIm8(A.high_8, HL.low_8);
            break;
        case 0x7E:
            loadIm8(A.high_8, memory->read(HL.all_16));
            break;
        case 0x7F:
            loadIm8(A.high_8, A.high_8);
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
            addA(A.high_8, false);
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
            addA(A.high_8, true);
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
            subA(A.high_8, false);
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
            subA(A.high_8, true);
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
            andA(A.high_8);
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
            xorA(A.high_8);
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
            orA(A.high_8);
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
            compareA(A.high_8);
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
            callZ(memory->read(PC), memory->read(PC + 1), false);
            PC += 2;
            break;
        case 0xC5:
            pushReg(BC);
            break;
        case 0xC6:
            addA(read_and_inc_pc(), false);
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
            callZ(memory->read(PC), memory->read(PC + 1), true);
            PC += 2;
            break;
        case 0xCD:
            call(memory->read(PC), memory->read(PC + 1));
            PC += 2;
            break;
        case 0xCE:
            addA(read_and_inc_pc(), true);
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
            callC(memory->read(PC), memory->read(PC + 1), false);
            PC += 2;
            break;
        case 0xD5:
            pushReg(DE);
            break;
        case 0xD6:
            subA(read_and_inc_pc(), false);
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
            callC(memory->read(PC), memory->read(PC + 1), true);
            PC += 2;
            break;
        case 0xDE:
            subA(read_and_inc_pc(), true);
            break;
        case 0xDF:
            reset(2);
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
            reset(3);
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
            popReg(A);
            F.all_8 = A.low_8;
            break;
        case 0xF5:
            A.low_8 = F.all_8;
            pushReg(A);
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

int CPU::CB_ops() {
    switch (read_and_inc_pc()) {
        case 0x00:
            rlc(BC.high_8);
            return 2;
        case 0x01:
            rlc(BC.low_8);
            return 2;
        case 0x02:
            rlc(DE.high_8);
            return 2;
        case 0x03:
            rlc(DE.low_8);
            return 2;
        case 0x04:
            rlc(HL.high_8);
            return 2;
        case 0x05:
            rlc(HL.low_8);
            return 2;
            //TODO 0X06
        case 0x07:
            rlc(A.high_8);
            return 2;
        case 0x08:
            rrc(BC.high_8);
            return 2;
        case 0x09:
            rrc(BC.low_8);
            return 2;
        case 0x0A:
            rrc(DE.high_8);
            return 2;
        case 0x0B:
            rrc(DE.low_8);
            return 2;
        case 0x0C:
            rrc(HL.high_8);
            return 2;
        case 0x0D:
            rrc(HL.low_8);
            return 2;

            //TODO 0x0E
        case 0x0F:
            rrc(A.high_8);
            return 2;
        case 0x10:
            rl(BC.high_8);
            return 2;
        case 0x11:
            rl(BC.low_8);
            return 2;
        case 0x12:
            rl(DE.high_8);
            return 2;
        case 0x13:
            rl(DE.low_8);
            return 2;
        case 0x14:
            rl(HL.high_8);
            return 2;
        case 0x15:
            rl(HL.low_8);
            return 2;
            //TODO 0X16
        case 0x17:
            rl(A.high_8);
            return 2;

        case 0x18:
            rr(BC.high_8);
            return 2;
        case 0x19:
            rr(BC.low_8);
            return 2;
        case 0x1A:
            rr(DE.high_8);
            return 2;
        case 0x1B:
            rr(DE.low_8);
            return 2;
        case 0x1C:
            rr(HL.high_8);
            return 2;
        case 0x1D:
            rr(HL.low_8);
            return 2;
            //TODO 0x1e
        case 0x1F:
            rr(A.high_8);
            return 2;

        case 0x40:
            bit(0, BC.high_8);
            return 2;
        case 0x41:
            bit(0, BC.low_8);
            return 2;
        case 0x42:
            bit(0, DE.high_8);
            return 2;
        case 0x43:
            bit(0, DE.low_8);
            return 2;
        case 0x44:
            bit(0, HL.high_8);
            return 2;
        case 0x45:
            bit(0, HL.low_8);
            return 2;
        case 0x46:
            bit(0, memory->read(HL.all_16));
            return 4;
        case 0x47:
            bit(0, A.high_8);
            return 2;
        case 0x48:
            bit(1, BC.high_8);
            return 2;
        case 0x49:
            bit(1, BC.low_8);
            return 2;
        case 0x4A:
            bit(1, DE.high_8);
            return 2;
        case 0x4B:
            bit(1, DE.low_8);
            return 2;
        case 0x4C:
            bit(1, HL.high_8);
            return 2;
        case 0x4D:
            bit(1, HL.low_8);
            return 2;
        case 0x4E:
            bit(1, memory->read(HL.all_16));
            return 4;
        case 0x4F:
            bit(1, A.high_8);
            return 2;
        case 0x50:
            bit(2, BC.high_8);
            return 2;
        case 0x51:
            bit(2, BC.low_8);
            return 2;
        case 0x52:
            bit(2, DE.high_8);
            return 2;
        case 0x53:
            bit(2, DE.low_8);
            return 2;
        case 0x54:
            bit(2, HL.high_8);
            return 2;
        case 0x55:
            bit(2, HL.low_8);
            return 2;
        case 0x56:
            bit(2, memory->read(HL.all_16));
            return 4;
        case 0x57:
            bit(2, A.high_8);
            return 2;
        case 0x58:
            bit(3, BC.high_8);
            return 2;
        case 0x59:
            bit(3, BC.low_8);
            return 2;
        case 0x5A:
            bit(3, DE.high_8);
            return 2;
        case 0x5B:
            bit(3, DE.low_8);
            return 2;
        case 0x5C:
            bit(3, HL.high_8);
            return 2;
        case 0x5D:
            bit(3, HL.low_8);
            return 2;
        case 0x5E:
            bit(3, memory->read(HL.all_16));
            return 4;
        case 0x5F:
            bit(3, A.high_8);
            return 2;
        case 0x60:
            bit(4, BC.high_8);
            return 2;
        case 0x61:
            bit(4, BC.low_8);
            return 2;
        case 0x62:
            bit(4, DE.high_8);
            return 2;
        case 0x63:
            bit(4, DE.low_8);
            return 2;
        case 0x64:
            bit(4, HL.high_8);
            return 2;
        case 0x65:
            bit(4, HL.low_8);
            return 2;
        case 0x66:
            bit(4, memory->read(HL.all_16));
            return 4;
        case 0x67:
            bit(4, A.high_8);
            return 2;
        case 0x68:
            bit(5, BC.high_8);
            return 2;
        case 0x69:
            bit(5, BC.low_8);
            return 2;
        case 0x6A:
            bit(5, DE.high_8);
            return 2;
        case 0x6B:
            bit(5, DE.low_8);
            return 2;
        case 0x6C:
            bit(5, HL.high_8);
            return 2;
        case 0x6D:
            bit(5, HL.low_8);
            return 2;
        case 0x6E:
            bit(5, memory->read(HL.all_16));
            return 4;
        case 0x6F:
            bit(5, A.high_8);
            return 2;
        case 0x70:
            bit(6, BC.high_8);
            return 2;
        case 0x71:
            bit(6, BC.low_8);
            return 2;
        case 0x72:
            bit(6, DE.high_8);
            return 2;
        case 0x73:
            bit(6, DE.low_8);
            return 2;
        case 0x74:
            bit(6, HL.high_8);
            return 2;
        case 0x75:
            bit(6, HL.low_8);
            return 2;
        case 0x76:
            bit(6, memory->read(HL.all_16));
            return 4;
        case 0x77:
            bit(6, A.high_8);
            return 2;
        case 0x78:
            bit(7, BC.high_8);
            return 2;
        case 0x79:
            bit(7, BC.low_8);
            return 2;
        case 0x7A:
            bit(7, DE.high_8);
            return 2;
        case 0x7B:
            bit(7, DE.low_8);
            return 2;
        case 0x7C:
            bit(7, HL.high_8);
            return 2;
        case 0x7D:
            bit(7, HL.low_8);
            return 2;
        case 0x7E:
            bit(7, memory->read(HL.all_16));
            return 4;
        case 0x7F:
            bit(7, A.high_8);
            return 2;

        case 0x80:
            res(0, BC.high_8);
            return 2;
        case 0x81:
            res(0, BC.low_8);
            return 2;
        case 0x82:
            res(0, DE.high_8);
            return 2;
        case 0x83:
            res(0, DE.low_8);
            return 2;
        case 0x84:
            res(0, HL.high_8);
            return 2;
        case 0x85:
            res(0, HL.low_8);
            return 2;
     //TODO 0x86
        case 0x87:
            res(0, A.high_8);
            return 2;
        case 0x88:
            res(1, BC.high_8);
            return 2;
        case 0x89:
            res(1, BC.low_8);
            return 2;
        case 0x8A:
            res(1, DE.high_8);
            return 2;
        case 0x8B:
            res(1, DE.low_8);
            return 2;
        case 0x8C:
            res(1, HL.high_8);
            return 2;
        case 0x8D:
            res(1, HL.low_8);
            return 2;
        //TODO 0X8E
        case 0x8F:
            res(1, A.high_8);
            return 2;

        case 0x90:
            res(2, BC.high_8);
            return 2;
        case 0x91:
            res(2, BC.low_8);
            return 2;
        case 0x92:
            res(2, DE.high_8);
            return 2;
        case 0x93:
            res(2, DE.low_8);
            return 2;
        case 0x94:
            res(2, HL.high_8);
            return 2;
        case 0x95:
            res(2, HL.low_8);
            return 2;
            //TODO 0x96
        case 0x97:
            res(2, A.high_8);
            return 2;
        case 0x98:
            res(3, BC.high_8);
            return 2;
        case 0x99:
            res(3, BC.low_8);
            return 2;
        case 0x9A:
            res(3, DE.high_8);
            return 2;
        case 0x9B:
            res(3, DE.low_8);
            return 2;
        case 0x9C:
            res(3, HL.high_8);
            return 2;
        case 0x9D:
            res(3, HL.low_8);
            return 2;
            //TODO 0X9E
        case 0x9F:
            res(3, A.high_8);
            return 2;
        case 0xA0:
            res(4, BC.high_8);
            return 2;
        case 0xA1:
            res(4, BC.low_8);
            return 2;
        case 0xA2:
            res(4, DE.high_8);
            return 2;
        case 0xA3:
            res(4, DE.low_8);
            return 2;
        case 0xA4:
            res(4, HL.high_8);
            return 2;
        case 0xA5:
            res(4, HL.low_8);
            return 2;
            //TODO 0x96
        case 0xA7:
            res(4, A.high_8);
            return 2;
        case 0xA8:
            res(5, BC.high_8);
            return 2;
        case 0xA9:
            res(5, BC.low_8);
            return 2;
        case 0xAA:
            res(5, DE.high_8);
            return 2;
        case 0xAB:
            res(5, DE.low_8);
            return 2;
        case 0xAC:
            res(5, HL.high_8);
            return 2;
        case 0xAD:
            res(5, HL.low_8);
            return 2;
            //TODO 0XAE
        case 0xAF:
            res(5, A.high_8);
            return 2;
        case 0xB0:
            res(6, BC.high_8);
            return 2;
        case 0xB1:
            res(6, BC.low_8);
            return 2;
        case 0xB2:
            res(6, DE.high_8);
            return 2;
        case 0xB3:
            res(6, DE.low_8);
            return 2;
        case 0xB4:
            res(6, HL.high_8);
            return 2;
        case 0xB5:
            res(6, HL.low_8);
            return 2;
            //TODO 0xB6
        case 0xB7:
            res(6, A.high_8);
            return 2;
        case 0xB8:
            res(7, BC.high_8);
            return 2;
        case 0xB9:
            res(7, BC.low_8);
            return 2;
        case 0xBA:
            res(7, DE.high_8);
            return 2;
        case 0xBB:
            res(7, DE.low_8);
            return 2;
        case 0xBC:
            res(7, HL.high_8);
            return 2;
        case 0xBD:
            res(7, HL.low_8);
            return 2;
            //TODO 0XBE
        case 0xBF:
            res(7, A.high_8);
            return 2;

        case 0xC0:
            set(0, BC.high_8);
            return 2;
        case 0xC1:
            set(0, BC.low_8);
            return 2;
        case 0xC2:
            set(0, DE.high_8);
            return 2;
        case 0xC3:
            set(0, DE.low_8);
            return 2;
        case 0xC4:
            set(0, HL.high_8);
            return 2;
        case 0xC5:
            set(0, HL.low_8);
            return 2;
            //TODO 0xC6
        case 0xC7:
            set(0, A.high_8);
            return 2;
        case 0xC8:
            set(1, BC.high_8);
            return 2;
        case 0xC9:
            set(1, BC.low_8);
            return 2;
        case 0xCA:
            set(1, DE.high_8);
            return 2;
        case 0xCB:
            set(1, DE.low_8);
            return 2;
        case 0xCC:
            set(1, HL.high_8);
            return 2;
        case 0xCD:
            set(1, HL.low_8);
            return 2;
            //TODO 0XCE
        case 0xCF:
            set(1, A.high_8);
            return 2;
        case 0xD0:
            set(2, BC.high_8);
            return 2;
        case 0xD1:
            set(2, BC.low_8);
            return 2;
        case 0xD2:
            set(2, DE.high_8);
            return 2;
        case 0xD3:
            set(2, DE.low_8);
            return 2;
        case 0xD4:
            set(2, HL.high_8);
            return 2;
        case 0xD5:
            set(2, HL.low_8);
            return 2;
            //TODO 0xD6
        case 0xD7:
            set(2, A.high_8);
            return 2;
        case 0xD8:
            set(3, BC.high_8);
            return 2;
        case 0xD9:
            set(3, BC.low_8);
            return 2;
        case 0xDA:
            set(3, DE.high_8);
            return 2;
        case 0xDB:
            set(3, DE.low_8);
            return 2;
        case 0xDC:
            set(3, HL.high_8);
            return 2;
        case 0xDD:
            set(3, HL.low_8);
            return 2;
            //TODO 0XDE
        case 0xDF:
            set(3, A.high_8);
            return 2;

        case 0xE0:
            set(4, BC.high_8);
            return 2;
        case 0xE1:
            set(4, BC.low_8);
            return 2;
        case 0xE2:
            set(4, DE.high_8);
            return 2;
        case 0xE3:
            set(4, DE.low_8);
            return 2;
        case 0xE4:
            set(4, HL.high_8);
            return 2;
        case 0xE5:
            set(4, HL.low_8);
            return 2;
            //TODO 0xE6
        case 0xE7:
            set(4, A.high_8);
            return 2;
        case 0xE8:
            set(5, BC.high_8);
            return 2;
        case 0xE9:
            set(5, BC.low_8);
            return 2;
        case 0xEA:
            set(5, DE.high_8);
            return 2;
        case 0xEB:
            set(5, DE.low_8);
            return 2;
        case 0xEC:
            set(5, HL.high_8);
            return 2;
        case 0xED:
            set(5, HL.low_8);
            return 2;
            //TODO 0XEE
        case 0xEF:
            set(5, A.high_8);
            return 2;
        case 0xF0:
            set(6, BC.high_8);
            return 2;
        case 0xF1:
            set(6, BC.low_8);
            return 2;
        case 0xF2:
            set(6, DE.high_8);
            return 2;
        case 0xF3:
            set(6, DE.low_8);
            return 2;
        case 0xF4:
            set(6, HL.high_8);
            return 2;
        case 0xF5:
            set(6, HL.low_8);
            return 2;
            //TODO 0xD6
        case 0xF7:
            set(6, A.high_8);
            return 2;
        case 0xF8:
            set(7, BC.high_8);
            return 2;
        case 0xF9:
            set(7, BC.low_8);
            return 2;
        case 0xFA:
            set(7, DE.high_8);
            return 2;
        case 0xFB:
            set(7, DE.low_8);
            return 2;
        case 0xFC:
            set(7, HL.high_8);
            return 2;
        case 0xFD:
            set(7, HL.low_8);
            return 2;
            //TODO 0XFE
        case 0xFF:
            set(7, A.high_8);
            return 2;

    }
    return 0;
}
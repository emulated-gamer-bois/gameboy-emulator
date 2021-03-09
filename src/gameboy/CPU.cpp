//
// Created by davidm on 2021-02-12.
//

#include "CPU.h"
#include "MMU.h"
#include <iostream> // cpu_dump
#include <iomanip> // cpu_dump
#include "Definitions.h"

uint16_t combine_bytes(uint8_t first_byte, uint8_t second_byte);

CPU::CPU(uint16_t PC, uint16_t SP, std::shared_ptr<MMU> mmu) {
    this->PC = PC;
    this->memory = mmu;
    this->SP.all_16 = SP;
    A = 0x00;
    BC.all_16 = 0x00;
    DE.all_16 = 0x00;
    HL.all_16 = 0x00;
    F.all_8 = 0x0;
    stop = false;
    halt = false;
    IME = 0;

}

void CPU::reset() {
    this->PC = 0x0000;
    this->SP.all_16 = 0xFFFE;
    A = 0x00;
    BC.all_16 = 0x00;
    DE.all_16 = 0x00;
    HL.all_16 = 0x00;
    F.all_8 = 0x0;
    IME = 0;
}

void CPU::cpu_dump() {
    std::cout << "=---------------------------=" << std::endl;
    std::cout << "A: 0x" << std::hex << (int) this->A << std::endl;
    std::cout << "BC: 0x" << std::hex << this->BC.all_16 << std::endl;
    std::cout << "DE: 0x" << std::hex << this->DE.all_16 << std::endl;
    std::cout << "HL: 0x" << std::hex << this->HL.all_16 << std::endl;
    std::cout << "F: 0x" << std::hex << (int) this->F.all_8 << std::endl;
    std::cout << "PC: 0x" << std::hex << this->PC << std::endl;
    std::cout << "SP: 0x" << std::hex << this->SP.all_16 << std::endl;
    std::cout << "=---------------------------=" << std::endl;
}

int CPU::update() {
    /*if (!this->cpu->getStop()) {
        // TODO: Check for interrupts
        int cycles = 0;
        if (!this->cpu->getHalt()) {
            cycles = this->cpu->execute_instruction();

        }else if(mmu->read(0xffff) & mmu->read(0xFF0F) & 0x1f){

        }
        this->ppu->update(cycles);
        this->mmu->timer_update(cycles);
    } else if (mmu->read(0xFFFF)) {
        cpu->return_from_stop();
    }*/

    if (this->isInterrupted()) {
        return this->handleInterrupts();
    }

    if (this->halt) {
        return 1;
    }

    return this->execute_instruction();
}

void CPU::skipBootRom() {
    this->PC = 0x0100;
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
 * @param subtraction if subtraction is performed
 * @param cFlag the value of the C flag, 0 if the cFlag is not used
 */
void CPU::setHFlag(uint8_t a, uint8_t b, bool subtraction, uint8_t cFlag) {
    if (subtraction) {
        // Sets the H flag if carry from bit 4 to bit 3
        F.h = ((a & 0x0F) - (b & 0x0F) - cFlag) < 0 ? 1 : 0;
    } else {
        // Sets the H flag if carry from bit 3 to bit 4
        F.h = (((a & 0x0F) + (b & 0x0F) + cFlag) & 0x10) == 0x10 ? 1 : 0;
    }
}

/**
 * Sets C flag if there is a carry between bit 7 and bit 8
 * @param a parameter 1 in addition
 * @param b parameter 2 in addition
 * @param subtraction if the operation executed is subtraction
 */
void CPU::setCFlag(uint16_t a, uint16_t b, bool subtraction) {
    // Sets the C flag if overflow
    if (subtraction) {
        //Inverted C flag if using subtraction
        F.c = (a + b) & 0x100 ? 0 : 1;
    } else {
        F.c = (a + b) > 0xFF ? 1 : 0;
    }
}

/**
 * Loads the immediate value of the two bytes into the provided register
 * @param firstByte the first argument to the operation code, will be loaded to the lower part of the register
 * @param secondByte the second argument to the operation code, will be loaded to the higher part of the register
 */
void CPU::loadIm16(uint16_t value, RegisterPair &reg) {
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
    add_8bit(A, value, withCarry);
}

void CPU::add_8bit(uint8_t &a, uint8_t b, bool withCarry) {
    auto CFlag = withCarry ? F.c : 0;
    setCFlag(a, b + CFlag, false);
    setHFlag(a, b, false, CFlag);
    a += b + CFlag;
    setZNFlags(a, false);
}

/**
 * This is ugly as shit, but this seems to more or less be how the gameboy implements it.
 * https://stackoverflow.com/questions/57958631/game-boy-half-carry-flag-and-16-bit-instructions-especially-opcode-0xe8
 * */
void CPU::addHL(RegisterPair reg) {
    auto tempZ = F.z;
    add_8bit(HL.low_8, reg.low_8, false);
    add_8bit(HL.high_8, reg.high_8, true);
    F.z = tempZ;
    F.n = 0;
}

void CPU::addSignedToRegPair(RegisterPair &regPair, int8_t value) {
    add_8bit(regPair.low_8, value, false);
    auto tmpH = F.h;
    auto tmpC = F.c;
    add_8bit(regPair.high_8, value & 0x80 ? 0xFF : 0x00, true);
    F.z = 0;
    F.n = 0;
    F.h = tmpH;
    F.c = tmpC;
}


/**
 * Makes a 8-bit number negative by converting it to two complement
 */
uint16_t twosComp8(uint16_t value) {
    return (value ^ 0xFF) + 1;
}

/**
 * Executes SUB with the A register and the given value
 * stores the result in A. Can be done with or without carry,carry
 * currently subtracts 1 if set.
 */
void CPU::subA(uint8_t value, bool withCarry) {
    auto CFlag = withCarry ? F.c : 0;
    setHFlag(A, value, true, CFlag);
    setCFlag(A, twosComp8(value + CFlag), true);
    A += twosComp8(value + CFlag);
    setZNFlags(A, true);
}

/**
* Increment the value stored at address @addr
 */
void CPU::incrementAddr(uint16_t addr) {
    uint8_t value = memory->read(addr);
    setHFlag(value++, 1, false, 0);
    memory->write(addr, value);
    setZNFlags(value, false);
}

/**
* Decrement the value stored at address @addr
 */
void CPU::decrementAddr(uint16_t addr) {
    uint8_t value = memory->read(addr);
    setHFlag(value--, 1, true, 0);
    memory->write(addr, value);
    setZNFlags(value, true);
}

/**
 * Increment the value of the specified 16 bit register with one.
 * Does not set flags.
 */
void CPU::increment16(uint16_t &reg) {
    reg += 1;
}

/**
 * Increment the value of the specified 8 bit register with one
 */
void CPU::increment8(uint8_t &reg) {
    setZNFlags(reg + 1, false);
    setHFlag(reg, 0x1, false, 0);
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
    setHFlag(addr, 1, true, 0);
    addr -= 1;
}

/**
 * Executes AND with the A register and the given value
 * stores the result in A
 */
void CPU::andA(uint8_t value) {
    A &= value;
    setZNFlags(A, false);

    //Sets H flag = 1, C = 0
    F.h = 1;
    F.c = 0;

}

/**
 * Executes XOR with the A register and the given value
 * stores the result in A
 */
void CPU::xorA(uint8_t value) {
    A ^= value;
    setZNFlags(A, false);

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
    A |= value;
    setZNFlags(A, false);

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

    F.all_8 = 0;
    //Sets C flag to d7
    F.c = d7 == 0 ? 0 : 1;
    F.z = reg == 0 ? 1 : 0;
}

/**
 * Rotate left, C -> d0 and d7 -> C
 */
void CPU::rl(uint8_t &reg) {
    auto d7 = (reg & 0x80) >> 0x07;
    reg = (reg << 1) | ((F.all_8 >> 4) & 0x01);

    F.all_8 = 0;
    //Sets C flag to d7
    F.c = d7 == 0 ? 0 : 1;
    F.z = reg == 0 ? 1 : 0;
}

/**
* Rotate right, d0 -> C flag and d0 -> d7
*/
void CPU::rrc(uint8_t &reg) {
    auto d0 = reg & 0x01;
    reg = (reg >> 1) | (d0 << 7);

    F.all_8 = 0;
    //Sets C flag to d0
    F.c = d0 == 0 ? 0 : 1;
    F.z = reg ? 0 : 1;
}

/**
 * Rotate left, C -> d7 and d0 -> C
 */
void CPU::rr(uint8_t &reg) {
    auto d0 = reg & 0x01;
    reg >>= 1;
    reg |= (F.all_8 << 3) & 0x80;

    setZNFlags(reg, false);
    F.h = 0;
    //Sets C flag to d0
    F.c = d0;
}

/**
 * Executes subtraction between value and register A and sets flags accordingly
 * the result is not saved
 */
void CPU::compareA(uint8_t value) {
    setHFlag(A, value, true, 0);
    setCFlag(A, twosComp8(value), true);
    setZNFlags(A + twosComp8(value), true);
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
 * @returns true if it jumps
 */
bool CPU::jumpZ(uint16_t addr, bool if_one) {
    if (!if_one == !(F.z)) {
        PC = addr;
        return true;
    } else
        return false;
}

/**
 * Jumps immediately to the specified address if C is one or zero
 * depending on the if_one parameter
 * @returns true if it jumps
 */
bool CPU::jumpC(uint16_t addr, bool if_one) {
    if (!if_one == !(F.c)) {
        PC = addr;
        return true;
    } else
        return false;
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
 * @returns true if it jumps
 */
bool CPU::jumpRelativeZ(int8_t steps, bool if_one) {
    if (!if_one == !(F.z)) {
        PC += steps;
        return true;
    } else
        return false;
}

/**
 * Increments PC with the given number of steps if C is one or zero
 * depending on the if_one parameter
 */
bool CPU::jumpRelativeC(int8_t steps, bool if_one) {
    if (!if_one == !(F.c)) {
        PC += steps;
        return true;
    } else
        return false;
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
 * @returns true if subroutine is called
 */
bool CPU::callZ(uint8_t firstByte, uint8_t secondByte, bool if_one) {
    if (!if_one != !(F.z))
        return false;
    else {
        call(firstByte, secondByte);
        return true;
    }
}

/**
 * Calls subroutine if C is one or zero
 * depending on the if_one parameter
 * @param firstByte fist parameter and the lower byte
 * @param secondByte second parameter the higher byte
 * @param if_one true if C should be 1, false if C should be 0
 */
bool CPU::callC(uint8_t firstByte, uint8_t secondByte, bool if_one) {
    if (!if_one != !(F.c))
        return false;
    else {
        call(firstByte, secondByte);
        return true;
    }

}

/**
 * Return from a subroutine by setting PC with
 * the two latest values on the stack.
 * Resets IME flag to 1 if returning from interrupt.
 * @param from_interrupt if the subroutine returns from interrupt
 */
void CPU::ret(bool from_interrupt) {
    PC = memory->read(SP.all_16++);
    PC |= memory->read(SP.all_16++) << 0x08;
    if (from_interrupt) {
        IME = 1;
    }
}

/**
 * Return from subroutine if Z is one or zero
 * depending on the if_one parameter
 * @param if_one true if Z should be 1, false if Z should be 0
 * @returns true if it returns from subroutine
 */
bool CPU::retZ(bool if_one) {
    if (!if_one != !(F.z))
        return false;
    else {
        ret(false);
        return true;
    }
}

/**
 * Return from subroutine if C is one or zero
 * depending on the if_one parameter
 * @param if_one true if C should be 1, false if C should be 0
 * @returns true if it returns from subroutine
 */
bool CPU::retC(bool if_one) {
    if (!if_one != !(F.c))
        return false;
    else {
        ret(false);
        return true;
    }
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
    reg = reg & ~(0x01 << bit_nr);
}

/**
 * Set bit_nr in reg.
 * * */
void CPU::set(uint8_t bit_nr, uint8_t &reg) {
    reg = reg | (0x01 << bit_nr);
}

/**
 * Every time we read PC, we want to increment it.
 * */
uint8_t CPU::read_and_inc_pc() {
    return memory->read(PC++);
}

void CPU::sla(uint8_t &reg) {
    F.c = reg >> 7;
    reg = reg << 1;
    reg &= 0xFE;
    setZNFlags(reg, false);
    F.h = 0;
}

void CPU::sra(uint8_t &reg) {
    auto b7 = (reg & 0x80);
    F.c = reg & 0x01;
    reg = reg >> 1;
    reg |= b7;
    setZNFlags(reg, false);
    F.h = 0;
}

void CPU::srl(uint8_t &reg) {
    F.c = reg & 0x1;
    reg >>= 1;
    F.h = 0;
    setZNFlags(reg, false);
}

/**
 * Is only used after addition or subtraction, is used to convert register A to Binary Coded Decimal(BCD).
 * This is done by first checking if the previous operation was addition or subtraction.
 * If it was addition, we check either if there is a carry, which indicates overflow(meaning that the value has passed 15, and is now misrepresenting the actual value it should represent, for example 18 --> should display 8, but shows 2 due to overflowing.
 * Or if the value of A>0x99, we simply check if the numerical value of the upper nibble is 10 or more, and it needs correction.
 * If it on the other hand is subtraction, the only time a value > 9 can be shown is if one of the carries occurred as this means that there was a borrow and the numbers need correction.
 * By correction I mean + or - with the values 0x60 or 0x06. This is due to 0xF-0x9=0x6, which is the correction needed to transform the values to BCD.
 * Credits to AWJ @ https://forums.nesdev.com/viewtopic.php?t=15944
 * */
void CPU::daa() {
    if (!F.n) {
        if (F.c || A > 0x99) {
            A += 0x60;
            F.c = 1;
        }
        if (F.h || (A & 0x0f) > 0x09) { A += 0x6; }
    } else {  // after a subtraction, only adjust if (half-)carry occurred
        if (F.c) { A -= 0x60; }
        if (F.h) { A -= 0x6; }
    }
// these flags are always updated
    F.z = (A == 0) ? 1 : 0; // the usual z flag
    F.h = 0; // h flag is always cleared
}

/**
 * Stops all execution. Is cancelled if any button is pressed. Resets IE flags, and sets all IO ports (P10-P13) to low.
 *
 * */
void CPU::stop_op() {

    loadIm8(BC.high_8, memory->read(INTERRUPT_ENABLE)); // Save IE
    memory->write(INTERRUPT_ENABLE, 0x00); //clear IE
    memory->write(IO_START, memory->read(IO_START) & 0xF0);
    stop = true;

}

void CPU::return_from_stop() {
    loadIm8(A, BC.high_8);
    memory->write(0xffff, A);
    stop = false;
}

bool CPU::getStop() {
    return stop;
}

bool CPU::getHalt() {
    return halt;
}
/**
 * Stops system clock(aka CPU), is reset after IE and and IF flags are set in bitwise pairs.
 * Simply continues after halt is over.
 *
 * */
void CPU::halt_op() {
    if (IME != 0x0000) {
        halt = true;
    } else if (memory->read(INTERRUPT_ENABLE & INTERRUPT_FLAG & 0x1F)) {
        //HALT MODE ENTERED
        halt = true;
    }
    //TODO If none of these happen the halt bug should occur.
}

void CPU::cpl() {
    A = ~A;
    F.n = 1;
    F.h = 1;
}

void CPU::ccf() {
    F.c = !F.c;
    F.n = 0;
    F.h = 0;
}

/**
 * Swaps the value of the lower 4 bits (bit 0 to 3) with
 * the value of the higher 4 bits (bit 4 to 7)
 * @param value the value which should be swapped
 * @return the swapped value of 'value'
 */
uint8_t CPU::swapBits(uint8_t value) {
    F.all_8 = 0;
    uint8_t newVal = value << 4;
    newVal |= (value >> 4);
    setZNFlags(newVal, false);
    return newVal;
}

/**
 * Reads the two upcoming bytes, returns their combined value and incs PC twice
 * The first byte is the lower byte
 * The second byte is the higher byte
 * @return
 */
uint16_t CPU::read16_and_inc_pc() {
    PC += 2;
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
 * @returns amount of machine cycles operation takes.
 */
int CPU::execute_instruction() {
    RegisterPair tmpReg;
    switch (read_and_inc_pc()) {
        case 0x00:
            nop();
            return 1;
        case 0x01:
            loadIm16(read16_and_inc_pc(), BC);
            return 3;
        case 0x02:
            storeAddr(BC.all_16, A);
            return 2;
        case 0x03:
            increment16(BC.all_16);
            return 2;
        case 0x04:
            increment8(BC.high_8);
            return 1;
        case 0x05:
            decrement8(BC.high_8);
            return 1;
        case 0x06:
            loadIm8(BC.high_8, read_and_inc_pc());
            return 2;
        case 0x07: //RLCA
            rlc(A);
            F.z = 0;
            return 1;
        case 0x08:
            //special case, not using read_and_inc PC
            storeAddr(combine_bytes(memory->read(PC), memory->read(PC + 1)), SP.low_8);
            storeAddr(combine_bytes(memory->read(PC), memory->read(PC + 1)) + 1, SP.high_8);
            PC += 2;
            return 5;
        case 0x09:
            addHL(BC);
            return 2;
        case 0x0A:
            loadImp(BC.all_16, A);
            return 2;
        case 0x0B:
            decrement16(BC.all_16);
            return 2;
        case 0x0C:
            increment8(BC.low_8);
            return 1;
        case 0x0D:
            decrement8(BC.low_8);
            return 1;
        case 0x0E:
            loadIm8(BC.low_8, read_and_inc_pc());
            return 2;
        case 0x0F: //RRCA
            rrc(A);
            F.all_8 &= 0x10;
            return 1;
        case 0x10:
            stop_op();
            //Says it should take 2 bytes.
            PC++;
            return 1;
        case 0x11:
            loadIm16(read16_and_inc_pc(), DE);
            return 1;
        case 0x12:
            storeAddr(DE.all_16, A);
            return 3;
        case 0x13:
            increment16(DE.all_16);
            return 2;
        case 0x14:
            increment8(DE.high_8);
            return 2;
        case 0x15:
            decrement8(DE.high_8);
            return 1;
        case 0x16:
            loadIm8(DE.high_8, read_and_inc_pc());
            return 2;
        case 0x17: //RLA
            rl(A);
            F.z = 0;
            return 1;
        case 0x18:
            jumpRelative(read_and_inc_pc());
            return 3;
        case 0x19:
            addHL(DE);
            return 2;
        case 0x1A:
            loadImp(DE.all_16, A);
            return 2;
        case 0x1B:
            decrement16(DE.all_16);
            return 2;
        case 0x1C:
            increment8(DE.low_8);
            return 1;
        case 0x1D:
            decrement8(DE.low_8);
            return 1;
        case 0x1E:
            loadIm8(DE.low_8, read_and_inc_pc());
            return 2;
        case 0x1F: //RRA
            rr(A);
            F.z = 0; // Special case
            return 1;
        case 0x20:
            if (jumpRelativeZ(read_and_inc_pc(), false))
                return 3;
            else
                return 2;
        case 0x21:

            loadIm16(read16_and_inc_pc(), HL);
            return 3;
        case 0x22:
            storeAddr(HL.all_16, A);
            increment16(HL.all_16);
            return 2;
        case 0x23:
            increment16(HL.all_16);
            return 2;
        case 0x24:
            increment8(HL.high_8);
            return 1;
        case 0x25:
            decrement8(HL.high_8);
            return 1;
        case 0x26:
            loadIm8(HL.high_8, read_and_inc_pc());
            return 2;
        case 0x27:
            daa();
            return 1;
        case 0x28:
            if (jumpRelativeZ(read_and_inc_pc(), true))
                return 3;
            else
                return 2;
        case 0x29:
            addHL(HL);
            return 2;
        case 0x2A:
            loadImp(HL.all_16, A);
            increment16(HL.all_16);
            return 2;
        case 0x2B:
            decrement16(HL.all_16);
            return 2;
        case 0x2C:
            increment8(HL.low_8);
            return 1;
        case 0x2D:
            decrement8(HL.low_8);
            return 1;
        case 0x2E:
            loadIm8(HL.low_8, read_and_inc_pc());
            return 2;
        case 0x2F:
            cpl();
            return 1;
        case 0x30:
            if (jumpRelativeC(read_and_inc_pc(), false))
                return 3;
            else
                return 2;
        case 0x31:
            loadIm16(read16_and_inc_pc(), SP);
            return 3;
        case 0x32:
            storeAddr(HL.all_16, A);
            decrement16(HL.all_16);
            return 2;
        case 0x33:
            increment16(SP.all_16);
            return 2;
        case 0x34:
            incrementAddr(HL.all_16);
            return 3;
        case 0x35:
            decrementAddr(HL.all_16);
            return 3;
        case 0x36:
            storeAddr(HL.all_16, read_and_inc_pc());
            return 3;
        case 0x37:
            F.all_8 &= 0x80;
            F.c = 1;
            return 1;
        case 0x38:
            if (jumpRelativeC(read_and_inc_pc(), true))
                return 3;
            else
                return 2;
        case 0x39:
            addHL(SP);
            return 2;
        case 0x3A:
            loadImp(HL.all_16, A);
            decrement16(HL.all_16);
            return 2;
        case 0x3B:
            decrement16(SP.all_16);
            return 2;
        case 0x3C:
            increment8(A);
            return 1;
        case 0x3D:
            decrement8(A);
            return 1;
        case 0x3E:
            loadIm8(A, read_and_inc_pc());
            return 2;
        case 0x3F:
            ccf();
            return 1;
        case 0x40:
            //This is stoopid.
            loadIm8(BC.high_8, BC.high_8);
            return 1;
        case 0x41:
            loadIm8(BC.high_8, BC.low_8);
            return 1;
        case 0x42:
            loadIm8(BC.high_8, DE.high_8);
            return 1;
        case 0x43:
            loadIm8(BC.high_8, DE.low_8);
            return 1;
        case 0x44:
            loadIm8(BC.high_8, HL.high_8);
            return 1;
        case 0x45:
            loadIm8(BC.high_8, HL.low_8);
            return 1;
        case 0x46:
            loadIm8(BC.high_8, memory->read(HL.all_16));
            return 2;
        case 0x47:
            loadIm8(BC.high_8, A);
            return 1;
        case 0x48:
            loadIm8(BC.low_8, BC.high_8);
            return 1;
        case 0x49:
            loadIm8(BC.low_8, BC.low_8);
            return 1;
        case 0x4A:
            loadIm8(BC.low_8, DE.high_8);
            return 1;
        case 0x4B:
            loadIm8(BC.low_8, DE.low_8);
            return 1;
        case 0x4C:
            loadIm8(BC.low_8, HL.high_8);
            return 1;
        case 0x4D:
            loadIm8(BC.low_8, HL.low_8);
            return 1;
        case 0x4E:
            loadIm8(BC.low_8, memory->read(HL.all_16));
            return 2;
        case 0x4F:
            loadIm8(BC.low_8, A);
            return 1;
        case 0x50:
            loadIm8(DE.high_8, BC.high_8);
            return 1;
        case 0x51:
            loadIm8(DE.high_8, BC.low_8);
            return 1;
        case 0x52:
            loadIm8(DE.high_8, DE.high_8);
            return 1;
        case 0x53:
            loadIm8(DE.high_8, DE.low_8);
            return 1;
        case 0x54:
            loadIm8(DE.high_8, HL.high_8);
            return 1;
        case 0x55:
            loadIm8(DE.high_8, HL.low_8);
            return 1;
        case 0x56:
            loadIm8(DE.high_8, memory->read(HL.all_16));
            return 2;
        case 0x57:
            loadIm8(DE.high_8, A);
            return 1;
        case 0x58:
            loadIm8(DE.low_8, BC.high_8);
            return 1;
        case 0x59:
            loadIm8(DE.low_8, BC.low_8);
            return 1;
        case 0x5A:
            loadIm8(DE.low_8, DE.high_8);
            return 1;
        case 0x5B:
            loadIm8(DE.low_8, DE.low_8);
            return 1;
        case 0x5C:
            loadIm8(DE.low_8, HL.high_8);
            return 1;
        case 0x5D:
            loadIm8(DE.low_8, HL.low_8);
            return 1;
        case 0x5E:
            loadIm8(DE.low_8, memory->read(HL.all_16));
            return 2;
        case 0x5F:
            loadIm8(DE.low_8, A);
            return 1;
        case 0x60:
            loadIm8(HL.high_8, BC.high_8);
            return 1;
        case 0x61:
            loadIm8(HL.high_8, BC.low_8);
            return 1;
        case 0x62:
            loadIm8(HL.high_8, DE.high_8);
            return 1;
        case 0x63:
            loadIm8(HL.high_8, DE.low_8);
            return 1;
        case 0x64:
            loadIm8(HL.high_8, HL.high_8);
            return 1;
        case 0x65:
            loadIm8(HL.high_8, HL.low_8);
            return 1;
        case 0x66:
            loadIm8(HL.high_8, memory->read(HL.all_16));
            return 2;
        case 0x67:
            loadIm8(HL.high_8, A);
            return 1;
        case 0x68:
            loadIm8(HL.low_8, BC.high_8);
            return 1;
        case 0x69:
            loadIm8(HL.low_8, BC.low_8);
            return 1;
        case 0x6A:
            loadIm8(HL.low_8, DE.high_8);
            return 1;
        case 0x6B:
            loadIm8(HL.low_8, DE.low_8);
            return 1;
        case 0x6C:
            loadIm8(HL.low_8, HL.high_8);
            return 1;
        case 0x6D:
            loadIm8(HL.low_8, HL.low_8);
            return 1;
        case 0x6E:
            loadIm8(HL.low_8, memory->read(HL.all_16));
            return 2;
        case 0x6F:
            loadIm8(HL.low_8, A);
            return 1;
        case 0x70:
            storeAddr(HL.all_16, BC.high_8);
            return 2;
        case 0x71:
            storeAddr(HL.all_16, BC.low_8);
            return 2;
        case 0x72:
            storeAddr(HL.all_16, DE.high_8);
            return 2;
        case 0x73:
            storeAddr(HL.all_16, DE.low_8);
            return 2;
        case 0x74:
            storeAddr(HL.all_16, HL.high_8);
            return 2;
        case 0x75:
            storeAddr(HL.all_16, HL.low_8);
            return 2;
        case 0x76:
            halt_op();
            return 1;
        case 0x77:
            storeAddr(HL.all_16, A);
            return 2;
        case 0x78:
            loadIm8(A, BC.high_8);
            return 1;
        case 0x79:
            loadIm8(A, BC.low_8);
            return 1;
        case 0x7A:
            loadIm8(A, DE.high_8);
            return 1;
        case 0x7B:
            loadIm8(A, DE.low_8);
            return 1;
        case 0x7C:
            loadIm8(A, HL.high_8);
            return 1;
        case 0x7D:
            loadIm8(A, HL.low_8);
            return 1;
        case 0x7E:
            loadIm8(A, memory->read(HL.all_16));
            return 2;
        case 0x7F:
            loadIm8(A, A);
            return 1;
        case 0x80:
            addA(BC.high_8, false);
            return 1;
        case 0x81:
            addA(BC.low_8, false);
            return 1;
        case 0x82:
            addA(DE.high_8, false);
            return 1;
        case 0x83:
            addA(DE.low_8, false);
            return 1;
        case 0x84:
            addA(HL.high_8, false);
            return 1;
        case 0x85:
            addA(HL.low_8, false);
            return 1;
        case 0x86:
            addA(memory->read(HL.all_16), false);
            return 2;
        case 0x87:
            addA(A, false);
            return 1;
        case 0x88:
            addA(BC.high_8, true);
            return 2;
        case 0x89:
            addA(BC.low_8, true);
            return 2;
        case 0x8A:
            addA(DE.high_8, true);
            return 2;
        case 0x8B:
            addA(DE.low_8, true);
            return 2;
        case 0x8C:
            addA(HL.high_8, true);
            return 2;
        case 0x8D:
            addA(HL.low_8, true);
            return 2;
        case 0x8E:
            addA(memory->read(HL.all_16), true);
            return 2;
        case 0x8F:
            addA(A, true);
            return 2;
        case 0x90:
            subA(BC.high_8, false);
            return 1;
        case 0x91:
            subA(BC.low_8, false);
            return 1;
        case 0x92:
            subA(DE.high_8, false);
            return 1;
        case 0x93:
            subA(DE.low_8, false);
            return 1;
        case 0x94:
            subA(HL.high_8, false);
            return 1;
        case 0x95:
            subA(HL.low_8, false);
            return 1;
        case 0x96:
            subA(memory->read(HL.all_16), false);
            return 2;
        case 0x97:
            subA(A, false);
            return 1;
        case 0x98:
            subA(BC.high_8, true);
            return 1;
        case 0x99:
            subA(BC.low_8, true);
            return 1;
        case 0x9A:
            subA(DE.high_8, true);
            return 1;
        case 0x9B:
            subA(DE.low_8, true);
            return 1;
        case 0x9C:
            subA(HL.high_8, true);
            break;
        case 0x9D:
            subA(HL.low_8, true);
            return 1;
        case 0x9E:
            subA(memory->read(HL.all_16), true);
            return 2;
        case 0x9F:
            subA(A, true);
            return 1;
        case 0xA0:
            andA(BC.high_8);
            return 1;
        case 0xA1:
            andA(BC.low_8);
            return 1;
        case 0xA2:
            andA(DE.high_8);
            return 1;
        case 0xA3:
            andA(DE.low_8);
            return 1;
        case 0xA4:
            andA(HL.high_8);
            return 1;
        case 0xA5:
            andA(HL.low_8);
            return 1;
        case 0xA6:
            andA(memory->read(HL.all_16));
            return 2;
        case 0xA7:
            andA(A);
            return 1;
        case 0xA8:
            xorA(BC.high_8);
            return 1;
        case 0xA9:
            xorA(BC.low_8);
            return 1;
        case 0xAA:
            xorA(DE.high_8);
            return 1;
        case 0xAB:
            xorA(DE.low_8);
            return 1;
        case 0xAC:
            xorA(HL.high_8);
            return 1;
        case 0xAD:
            xorA(HL.low_8);
            return 1;
        case 0xAE:
            xorA(memory->read(HL.all_16));
            return 2;
        case 0xAF:
            xorA(A);
            return 1;
        case 0xB0:
            orA(BC.high_8);
            return 1;
        case 0xB1:
            orA(BC.low_8);
            return 1;
        case 0xB2:
            orA(DE.high_8);
            return 1;
        case 0xB3:
            orA(DE.low_8);
            return 1;
        case 0xB4:
            orA(HL.high_8);
            return 1;
        case 0xB5:
            orA(HL.low_8);
            return 1;
        case 0xB6:
            orA(memory->read(HL.all_16));
            return 2;
        case 0xB7:
            orA(A);
            return 1;
        case 0xB8:
            compareA(BC.high_8);
            return 1;
        case 0xB9:
            compareA(BC.low_8);
            return 1;
        case 0xBA:
            compareA(DE.high_8);
            return 1;
        case 0xBB:
            compareA(DE.low_8);
            return 1;
        case 0xBC:
            compareA(HL.high_8);
            return 1;
        case 0xBD:
            compareA(HL.low_8);
            return 1;
        case 0xBE:
            compareA(memory->read(HL.all_16));
            return 2;
        case 0xBF:
            compareA(A);
            return 1;
        case 0xC0:
            if (retZ(false))
                return 5;
            else
                return 2;
        case 0xC1:
            popReg(BC);
            return 3;
        case 0xC2:
            if (jumpZ(read16_and_inc_pc(), false))
                return 4;
            else
                return 3;
        case 0xC3:
            jump(read16_and_inc_pc());
            return 4;
        case 0xC4:
            PC += 2;
            if (callZ(memory->read(PC - 2), memory->read(PC - 1), false)) {
                return 6;
            } else {
                return 3;
            }
        case 0xC5:
            pushReg(BC);
            return 4;
        case 0xC6:
            addA(read_and_inc_pc(), false);
            return 2;
        case 0xC7:
            reset(0);
            return 4;
        case 0xC8:
            if (retZ(true))
                return 5;
            else
                return 2;
        case 0xC9:
            ret(false);
            return 4;
        case 0xCA:
            if (jumpZ(read16_and_inc_pc(), true))
                return 4;
            else
                return 3;
        case 0xCB:
            return CB_ops();
        case 0xCC:
            PC += 2;
            if (callZ(memory->read(PC - 2), memory->read(PC - 1), true)) {
                return 6;
            } else {
                return 3;
            }
        case 0xCD:
            PC += 2;
            call(memory->read(PC - 2), memory->read(PC - 1));
            return 6;
        case 0xCE:
            addA(read_and_inc_pc(), true);
            return 2;
        case 0xCF:
            reset(1);
            return 4;
        case 0xD0:
            if (retC(false))
                return 5;
            else
                return 2;
        case 0xD1:
            popReg(DE);
            return 3;
        case 0xD2:
            if (jumpC(read16_and_inc_pc(), false))
                return 4;
            else
                return 3;
        case 0xD4:
            PC += 2;
            if (callC(memory->read(PC - 2), memory->read(PC - 1), false)) {
                return 6;
            } else {
                return 3;
            }
        case 0xD5:
            pushReg(DE);
            return 4;
        case 0xD6:
            subA(read_and_inc_pc(), false);
            return 2;
        case 0xD7:
            reset(2);
            return 4;
        case 0xD8:
            if (retC(true))
                return 5;
            else
                return 2;
        case 0xD9:
            ret(true);
            return 4;
        case 0xDA:
            if (jumpC(read16_and_inc_pc(), true))
                return 4;
            else
                return 3;
        case 0xDC:
            PC += 2;
            if (callC(memory->read(PC - 2), memory->read(PC - 1), true)) {
                return 6;
            } else {
                return 3;
            }
        case 0xDE:
            subA(read_and_inc_pc(), true);
            return 2;
        case 0xDF:
            reset(3);
            return 4;
        case 0xE0:
            storeAddr(0xFF00 + read_and_inc_pc(), A);
            return 3;
        case 0xE1:
            popReg(HL);
            return 3;
        case 0xE2:
            storeAddr(0xFF00 + BC.low_8, A);
            return 2;
        case 0xE5:
            pushReg(HL);
            return 4;
        case 0xE6:
            andA(read_and_inc_pc());
            return 2;
        case 0xE7:
            reset(4);
            return 4;
        case 0xE8:
            addSignedToRegPair(SP, read_and_inc_pc());
            return 4;
        case 0xE9:
            jump(HL.all_16);
            return 1;
        case 0xEA:
            memory->write(read16_and_inc_pc(), A);
            return 4;
        case 0xEE:
            xorA(read_and_inc_pc());
            return 2;
        case 0xEF:
            reset(5);
            return 4;
        case 0xF0:
            loadImp(0xFF00 + read_and_inc_pc(), A);
            return 3;
        case 0xF1:
            popReg(tmpReg);
            A = tmpReg.high_8;
            F.all_8 = tmpReg.low_8;
            return 3;
        case 0xF2:
            loadImp(0xFF00 + BC.low_8, A);
            return 2;
        case 0xF3:
            IME = 0;
            return 1;
        case 0xF5:
            tmpReg.high_8 = A;
            tmpReg.low_8 = F.all_8 & 0xF0;
            pushReg(tmpReg);
            return 4;
        case 0xF6:
            orA(read_and_inc_pc());
            return 2;
        case 0xF7:
            reset(6);
            return 4;
        case 0xF8:
            HL = SP;
            addSignedToRegPair(HL, read_and_inc_pc());
            return 3;
        case 0xF9:
            loadIm16(HL.all_16, SP);
            return 2;
        case 0xFA:
            loadImp(read16_and_inc_pc(), A);
            return 4;
        case 0xFB:
            IME = 1;
            return 1;
        case 0xFE:
            compareA(read_and_inc_pc());
            return 2;
        case 0xFF:
            reset(7);
            return 4;
        default:
            nop();
            return 1;
    }


}

int CPU::CB_ops() {
    uint8_t tmpVal = 0;
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
        case 0x06:
            tmpVal = memory->read(HL.all_16);
            rlc(tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0x07:
            rlc(A);
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
        case 0x0E:
            tmpVal = memory->read(HL.all_16);
            rrc(tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0x0F:
            rrc(A);
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
        case 0x16:
            tmpVal = memory->read(HL.all_16);
            rl(tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0x17:
            rl(A);
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
        case 0x1E:
            tmpVal = memory->read(HL.all_16);
            rr(tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0x1F:
            rr(A);
            return 2;
        case 0x20:
            sla(BC.high_8);
            return 2;
        case 0x21:
            sla(BC.low_8);
            return 2;
        case 0x22:
            sla(DE.high_8);
            return 2;
        case 0x23:
            sla(DE.low_8);
            return 2;
        case 0x24:
            sla(HL.high_8);
            return 2;
        case 0x25:
            sla(HL.low_8);
            return 2;
        case 0x26:
            tmpVal = memory->read(HL.all_16);
            sla(tmpVal);
            memory->write(HL.all_16, tmpVal);
            return 4;
        case 0x27:
            sla(A);
            return 2;
        case 0x28:
            sra(BC.high_8);
            return 2;
        case 0x29:
            sra(BC.low_8);
            return 2;
        case 0x2A:
            sra(DE.high_8);
            return 2;
        case 0x2B:
            sra(DE.low_8);
            return 2;
        case 0x2C:
            sra(HL.high_8);
            return 2;
        case 0x2D:
            sra(HL.low_8);
            return 2;
        case 0x2E:
            tmpVal = memory->read(HL.all_16);
            sra(tmpVal);
            memory->write(HL.all_16, tmpVal);
            return 4;
        case 0x2F:
            sra(A);
            return 2;
        case 0x30:
            BC.high_8 = swapBits(BC.high_8);
            return 2;
        case 0x31:
            BC.low_8 = swapBits(BC.low_8);
            return 2;
        case 0x32:
            DE.high_8 = swapBits(DE.high_8);
            return 2;
        case 0x33:
            DE.low_8 = swapBits(DE.low_8);
            return 2;
        case 0x34:
            HL.high_8 = swapBits(HL.high_8);
            return 2;
        case 0x35:
            HL.low_8 = swapBits(HL.low_8);
            return 2;
        case 0x36:
            storeAddr(HL.all_16, swapBits(memory->read(HL.all_16)));
            return 4;
        case 0x37:
            A = swapBits(A);
            return 2;
        case 0x38:
            srl(BC.high_8);
            return 2;
        case 0x39:
            srl(BC.low_8);
            return 2;
        case 0x3A:
            srl(DE.high_8);
            return 2;
        case 0x3B:
            srl(DE.low_8);
            return 2;
        case 0x3C:
            srl(HL.high_8);
            return 2;
        case 0x3D:
            srl(HL.low_8);
            return 2;
        case 0x3E:
            tmpVal = memory->read(HL.all_16);
            srl(tmpVal);
            memory->write(HL.all_16, tmpVal);
            return 4;
        case 0x3F:
            srl(A);
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
            bit(0, A);
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
            bit(1, A);
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
            bit(2, A);
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
            bit(3, A);
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
            bit(4, A);
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
            bit(5, A);
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
            bit(6, A);
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
            bit(7, A);
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
        case 0x86:
            tmpVal = memory->read(HL.all_16);
            res(0, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0x87:
            res(0, A);
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
        case 0x8E:
            tmpVal = memory->read(HL.all_16);
            res(1, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0x8F:
            res(1, A);
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
        case 0x96:
            tmpVal = memory->read(HL.all_16);
            res(2, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0x97:
            res(2, A);
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
        case 0x9E:
            tmpVal = memory->read(HL.all_16);
            res(3, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0x9F:
            res(3, A);
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
        case 0xA6:
            tmpVal = memory->read(HL.all_16);
            res(4, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0xA7:
            res(4, A);
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
        case 0xAE:
            tmpVal = memory->read(HL.all_16);
            res(5, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0xAF:
            res(5, A);
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
        case 0xB6:
            tmpVal = memory->read(HL.all_16);
            res(6, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0xB7:
            res(6, A);
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
        case 0xBE:
            tmpVal = memory->read(HL.all_16);
            res(7, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0xBF:
            res(7, A);
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
        case 0xC6:
            tmpVal = memory->read(HL.all_16);
            set(0, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0xC7:
            set(0, A);
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
        case 0xCE:
            tmpVal = memory->read(HL.all_16);
            set(1, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0xCF:
            set(1, A);
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
        case 0xD6:
            tmpVal = memory->read(HL.all_16);
            set(2, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0xD7:
            set(2, A);
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
        case 0xDE:
            tmpVal = memory->read(HL.all_16);
            set(3, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0xDF:
            set(3, A);
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
        case 0xE6:
            tmpVal = memory->read(HL.all_16);
            set(4, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0xE7:
            set(4, A);
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
        case 0xEE:
            tmpVal = memory->read(HL.all_16);
            set(5, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0xEF:
            set(5, A);
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
        case 0xF6:
            tmpVal = memory->read(HL.all_16);
            set(6, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0xF7:
            set(6, A);
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
        case 0xFE:
            tmpVal = memory->read(HL.all_16);
            set(7, tmpVal);
            storeAddr(HL.all_16, tmpVal);
            return 4;
        case 0xFF:
            set(7, A);
            return 2;

    }
    return 0;
}

bool CPU::isInterrupted() {
    if (IME) {
        uint8_t flags = memory->read(INTERRUPT_FLAG);
        uint8_t mask = memory->read(INTERRUPT_ENABLE);
        return flags & mask;
    }
    if (this->halt) {
        return true;
    }
    return false;
}

int CPU::handleInterrupts() {
    int cycles = 5;

    IME = 0;

    uint8_t PC_high_byte = PC >> 8;
    uint8_t PC_low_byte = PC & 0xFF;
    memory->write(--SP.all_16, PC_high_byte);
    memory->write(--SP.all_16, PC_low_byte);

    uint8_t flags = memory->read(INTERRUPT_FLAG);
    uint8_t mask = memory->read(INTERRUPT_ENABLE);
    uint8_t maskedFlags = flags & mask;

    uint16_t interruptVector = PC;

    if (maskedFlags & V_BLANK_IF_BIT) {
        memory->write(INTERRUPT_FLAG, flags & ~V_BLANK_IF_BIT);
        interruptVector = 0x40;
    } else if (maskedFlags & STAT_IF_BIT) {
        memory->write(INTERRUPT_FLAG, flags & ~STAT_IF_BIT);
        interruptVector = 0x48;
    } else if (maskedFlags & TIMER_IF_BIT) {
        memory->write(INTERRUPT_FLAG, flags & ~TIMER_IF_BIT);
        std::cout << (int)memory->read(INTERRUPT_FLAG) << std::endl;
        interruptVector = 0x50;
    } else if (maskedFlags & SERIAL_IF_BIT) {
        memory->write(INTERRUPT_FLAG, flags & ~SERIAL_IF_BIT);
        interruptVector = 0x58;
    } else if (maskedFlags & CONTROLLER_IF_BIT) {
        memory->write(INTERRUPT_FLAG, flags & ~CONTROLLER_IF_BIT);
        interruptVector = 0x60;
    }
    PC = interruptVector;

    if (this->halt) {
        this->halt = false;
        cycles++;
    }

    return cycles;
}


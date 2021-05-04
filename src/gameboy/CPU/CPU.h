#pragma once

#include "RegisterPair.h"
#include "../MMU/MMU.h"
#include "Flags.h"
#include "../Definitions.h" //IF-bits
#include <iostream> // cpuDump
#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test

class CPU {
public:

    CPU(std::shared_ptr<MMU> memory);
    void reset();
    /**
     * Fetches, decodes and executes the instruction at location PC, also checks interrupts and halt.
     * @returns amount of machine cycles operation takes.
     */
    int update();
    void skipBootRom();
    void cpuDump();
    bool getStop() const;
    void returnFromStop();

private:
    //Registers

    uint16_t PC{0x0000};
    RegisterPair SP{};
    uint8_t  A{0x00};
    RegisterPair BC{0x00};
    RegisterPair DE{0x00};
    RegisterPair HL{0x00};
    //Flags
    Flags F{0x0};
    //Interrupt Master Enable flag
    unsigned int IME : 1;
    //Memory
    std::shared_ptr<MMU> memory;
    //Clock handling
    bool stop{false};
    bool halt{false};


    //Update related functions
    /**
    * Fetches, decodes and executes the instruction at location PC
    * @returns amount of machine cycles operation takes.
     */
    int executeInstruction();
    bool isInterrupted();
    /**
     * Handles interrupts by saving relevant data such as SP and PC, then
     * jumps to interrupt handling depending on what interrupt is requested.
     * Returns number of cycles required and then continues normal work flow.
     *
     * */
    int handleInterrupts();


    //Flag management
    /**
    * Z set if newValue is 0
    * N set if subtraction is true
    */
    void setZNFlags(uint8_t value, bool subtraction);
    /**
    * Sets H flag if there is a carry between bit 3 and bit 4
    * @param a parameter 1 in addition
    * @param b parameter 2 in addition
    * @param subtraction if subtraction is performed
    * @param cFlag the value of the C flag, 0 if the cFlag is not used
    */
    void setHFlag(uint8_t a, uint8_t b, bool subtraction, uint8_t cFlag);
    /**
    * Sets C flag if there is a carry between bit 7 and bit 8
    * @param a parameter 1 in addition
    * @param b parameter 2 in addition
    * @param subtraction if the operation executed is subtraction
    */
    void setCFlag(uint16_t a,uint16_t b, bool subtraction);
    //Setting registers
    void setA(uint8_t val){ A=val;};
    void setB(uint8_t val){ BC.high_8=val;};

    //Register arithmetics
    /**
    * Executes ADD with the A register and the given value
    * stores the result in A. Can be done with or without carry.
    */
    void addA(uint8_t value, bool withCarry);

    /**
    * Executes SUB with the A register and the given value
    * stores the result in A. Can be done with or without carry.
     */
    void subA(uint8_t value, bool withCarry);

    void incrementAddr(uint16_t addr);
    void increment8(uint8_t &reg);
    void increment16(uint16_t &reg);

    void decrementAddr(uint16_t addr);
    void decrement8(uint8_t &reg);
    void decrement16(uint16_t &reg);
    void add8bit(uint8_t &reg, uint8_t b, bool withCarry);
    void addHL(RegisterPair reg);
    void addSignedToRegPair(RegisterPair &regPair, int8_t value);
    /**
     * Is only used after addition or subtraction, is used to convert register A to Binary Coded Decimal(BCD).
    * This is done by first checking if the previous operation was addition or subtraction.
     * If it was addition, we check either if there is a carry, which indicates overflow(meaning that the value has passed 15, and is now misrepresenting the actual value it should represent, for example 18 --> should display 8, but shows 2 due to overflowing.
     * Or if the value of A>0x99, we simply check if the numerical value of the upper nibble is 10 or more, and it needs correction.
     * If it on the other hand is subtraction, the only time a value > 9 can be shown is if one of the carries occurred as this means that there was a borrow and the numbers need correction.
     * By correction I mean + or - with the values 0x60 or 0x06. This is due to 0xF-0x9=0x6, which is the correction needed to transform the values to BCD.
     * Credits to AWJ @ https://forums.nesdev.com/viewtopic.php?t=15944
    * */
    void daa();

    //Bitwise operations
    /**
    * Executes AND with the A register and the given value
    * stores the result in A
     */
    void andA(uint8_t value);
    /**
    * Executes XOR with the A register and the given value
    * stores the result in A
    */
    void xorA(uint8_t value);
    /**
    * Executes OR with the A register and the given value
    * stores the result in A
     */
    void orA(uint8_t value);
    /**
    * Rotate left, d7 -> C flag and d7 -> d0
    */
    void rlc(uint8_t &reg);
    /**
    * Rotate left, C -> d0 and d7 -> C
    */
    void rl(uint8_t &reg);
    /**
    * Rotate right, d0 -> C flag and d0 -> d7
    */
    void rrc(uint8_t &reg);
    /**
    * Rotate left, C -> d7 and d0 -> C
    */
    void rr(uint8_t &reg);
    void cpl();
    void ccf();

    //Read and write memory

    /**
    * Used for LD (BC), A or similar
    * @param reg (BC)
    * @param value value fo A
    */
    void storeAddr(uint16_t addr, uint8_t value);
    /**
    * Loads the specified register with the value found on the specified address
    */
    void loadImp(uint16_t addr, uint8_t &reg);
    /**
    * Loads the immediate value of the two bytes into the provided register
    * @param firstByte the first argument to the operation code, will be loaded to the lower part of the register
    * @param secondByte the second argument to the operation code, will be loaded to the higher part of the register
    */
    void loadIm16(uint16_t value, RegisterPair &reg);
    /**
    * Loads the immediate value of the value of the op-code parameter
    */
    void loadIm8(uint8_t &reg, uint8_t firstByte);

    //Stack functions
    void popSP(RegisterPair &reg);
    void pushSP(RegisterPair &reg);

     /**
    * Jumps immediately to the specified address
    */
    void jump(uint16_t addr);
    /**
    * Jumps immediately to the specified address if Z is one or zero
    * depending on the if_one parameter
    * @returns true if it jumps
    */
    bool jumpZ(uint16_t addr, bool if_one);
    /**
    * Jumps immediately to the specified address if C is one or zero
    * depending on the if_one parameter
    * @returns true if it jumps
    */
    bool jumpC(uint16_t addr, bool if_one);
    /**
    * Increments PC with the given number of steps
    */
    void jumpRelative(int8_t steps);
    /**
     * Increments PC with the given number of steps if Z is one or zero
    * depending on the if_one parameter
    * @returns true if it jumps
    */
    bool jumpRelativeZ(int8_t steps, bool if_one);
    /**
    * Increments PC with the given number of steps if C is one or zero
    * depending on the if_one parameter
    */
    bool jumpRelativeC(int8_t steps, bool if_one);

    //Call subroutines and return
    /**
    * Return from a subroutine by setting PC with
    * the two latest values on the stack.
    * Resets IME flag to 1 if returning from interrupt.
    * @param from_interrupt if the subroutine returns from interrupt
    */
    void ret(bool from_interrupt);
    /**
    * Return from subroutine if Z is one or zero
    * depending on the if_one parameter
     * @param if_one true if Z should be 1, false if Z should be 0
     * @returns true if it returns from subroutine
     */
    bool retZ(bool if_one);
    bool retC(bool if_one);
    /**
    * Call sub routine
    * Stores value of PC on the stack and sets the value of PC to the immediate data
    * @param firstByte fist parameter and the lower byte
    * @param secondByte second parameter the higher byte
    */
    void call(uint8_t firstByte, uint8_t secondByte);
    /**
    * Calls subroutine if Z is one or zero
    * depending on the if_one parameter
    * @param firstByte fist parameter and the lower byte
    * @param secondByte second parameter the higher byte
    * @param if_one true if Z should be 1, false if Z should be 0
    * @returns true if subroutine is called
    */
    bool callZ(uint8_t firstByte, uint8_t secondByte, bool if_one);
    /**
     * Calls subroutine if C is one or zero
    * depending on the if_one parameter
     * @param firstByte fist parameter and the lower byte
    * @param secondByte second parameter the higher byte
     * @param if_one true if C should be 1, false if C should be 0
    */
    bool callC(uint8_t firstByte, uint8_t secondByte, bool if_one);

    //16 bit operations
    int CBOps();
    /**
    * Sets the Z flag to the complement of bit number bit_no from value
    * @param bit_no bit 0 to 7
    * @param value the value of the byte which should be used
    */
    void bit(uint8_t bit_nr, uint8_t value);
    /**
     * Reset bit_nr in reg.
    **/
    void res(uint8_t bit_nr, uint8_t &reg);
    void sla(uint8_t &reg);
    void sra(uint8_t &reg);
    void srl(uint8_t &reg);
    /**
    * Set bit_nr in reg.
    **/
    void set(uint8_t bit_nr, uint8_t &reg);
    /**
    * Swaps the value of the lower 4 bits (bit 0 to 3) with
    * the value of the higher 4 bits (bit 4 to 7)
    * @param value the value which should be swapped
    * @return the swapped value of 'value'
    */
    uint8_t swapBits(uint8_t value);

    //Other
    /**
    * Restart PC from the nth byte (n as in number)
    * pushes PC to the stack before resetting PC
    * @param nth_byte vale from 0 to 7 of how many bytes from 0 PC should start at
    */
    void reset(uint8_t nth_byte);
    /**
    * Executes subtraction between value and register A and sets flags accordingly
    * the result is not saved
    */
    void compareA(uint8_t value);
    /**
    * Every time we read PC, we want to increment it.
    **/
    uint8_t readAndIncPc();
    /**
    * Reads the two upcoming bytes, returns their combined value and incs PC twice
    * The first byte is the lower byte
    * The second byte is the higher byte
    * @return
     */
    uint16_t read16AndIncPc();
    /**
    * Stops all execution. Is cancelled if any button is pressed. Resets IE flags, and sets all IO ports (P10-P13) to low.
    *
    * */
    void stopOp();
    /**
    * Stops system clock(aka CPU), is reset after IE and and IF flags are set in bitwise pairs.
    * Simply continues after halt is over.
    *
    * */
    void haltOp();
    FRIEND_TEST(CPU, Execute_NOP_Instruction);
    FRIEND_TEST(CPU, Execute_LD_SP_D16_Instruction);
    FRIEND_TEST(CPU, FUNDAMENTAL_FUNCTIONS);
    FRIEND_TEST(CPU, sixteen_bit_ops);
    FRIEND_TEST(PPU, Print_test_rom);
    FRIEND_TEST(PPU, g_tile_rom);
};

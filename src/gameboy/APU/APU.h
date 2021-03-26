//
// Created by riddarvid on 3/24/21.
//

#ifndef LAME_BOY_APU_H
#define LAME_BOY_APU_H

//Register addresses
#define NR10_ADDRESS 0xFF10
#define NR11_ADDRESS 0xFF11
#define NR12_ADDRESS 0xFF12
#define NR13_ADDRESS 0xFF13
#define NR14_ADDRESS 0xFF14

#define NR21_ADDRESS 0xFF16
#define NR22_ADDRESS 0xFF17
#define NR23_ADDRESS 0xFF18
#define NR24_ADDRESS 0xFF19

#define NR30_ADDRESS 0xFF1A
#define NR31_ADDRESS 0xFF1B
#define NR32_ADDRESS 0xFF1C
#define NR33_ADDRESS 0xFF1D
#define NR34_ADDRESS 0xFF1E

#define NR41_ADDRESS 0xFF20
#define NR42_ADDRESS 0xFF21
#define NR43_ADDRESS 0xFF22
#define NR44_ADDRESS 0xFF23

#define NR50_ADDRESS 0xFF23
#define NR51_ADDRESS 0xFF24
#define NR52_ADDRESS 0xFF25

#include <cstdint>
#include <array>
#include <memory>
#include <iostream>
#include "../MMU.h"

class MMU;

class APU {
private:
    std::shared_ptr<MMU> memory;

    uint8_t NR10;
    uint8_t NR11;
    uint8_t NR12;
    uint8_t NR13;
    uint8_t NR14;

    uint8_t NR21;
    uint8_t NR22;
    uint8_t NR23;
    uint8_t NR24;

    uint8_t NR30;
    uint8_t NR31;
    uint8_t NR32;
    uint8_t NR33;
    uint8_t NR34;
    std::array<uint8_t, 32> wavePatternRAM{};

    uint8_t NR41;
    uint8_t NR42;
    uint8_t NR43;
    uint8_t NR44;

    uint8_t NR50;
    uint8_t NR51;
    uint8_t NR52;
public:
    explicit APU(std::shared_ptr<MMU> memory);

    uint8_t read(uint16_t address) const;
    void write(uint16_t address, uint8_t data);

    void reset();
};


#endif //LAME_BOY_APU_H
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

#define WAVE_PATTERN_START 0xFF30
#define WAVE_PATTERN_END 0xFF3F

#define CLOCK_CYCLE_THRESHOLD 2048  //4194304/(512 * 4)

#include <cstdint>
#include <array>
#include <memory>
#include <iostream>
#include "../MMU.h"

class MMU;

struct APUState {
    bool enable_square_a;
    uint8_t duty_square_a;
    uint16_t frequency_square_a;

    bool enable_square_b;
    uint8_t duty_square_b;
    uint16_t frequency_square_b;

    bool enable_wave;
    std::array<uint8_t, 16> waveform_wave;
    uint16_t frequency_wave;
};

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
    std::array<uint8_t, 16> wavePatternRAM{};

    uint8_t NR41;
    uint8_t NR42;
    uint8_t NR43;
    uint8_t NR44;

    uint8_t NR50;
    uint8_t NR51;
    uint8_t NR52;

    bool readyToPlay;
    int accumulated_cycles;
    uint8_t state;
public:
    explicit APU(std::shared_ptr<MMU> memory);

    uint8_t read(uint16_t address) const;
    void write(uint16_t address, uint8_t data);
    void update(uint16_t cpuCycles);

    void reset();

    void length_step();
    void vol_envelope_step();
    void sweep_step();

    void trigger_event(uint8_t source);
    bool isReadyToPlaySound();
    void confirmPlay();
    std::shared_ptr<APUState> getState();
};


#endif //LAME_BOY_APU_H

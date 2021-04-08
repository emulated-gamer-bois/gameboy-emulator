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

#define NR50_ADDRESS 0xFF24
#define NR51_ADDRESS 0xFF25
#define NR52_ADDRESS 0xFF26

#define WAVE_PATTERN_START 0xFF30
#define WAVE_PATTERN_END 0xFF3F

#define CLOCK_CYCLE_THRESHOLD 2048  //4194304/(512 * 4)

#include <cstdint>
#include <array>
#include <memory>
#include <iostream>
#include "../MMU/MMU.h"
#include "APUState.h"
#include "IVolumeController.h"

class APU {
public:
    explicit APU();

    uint8_t read(uint16_t address) const;
    void write(uint16_t address, uint8_t data);
    void update(uint16_t cpuCycles, IVolumeController* vc);

    void reset();

    void length_step();
    void vol_envelope_step(IVolumeController* vc);
    void sweep_step();

    void trigger_event(uint8_t source);
    uint8_t isReadyToPlaySound();
    void confirmPlay();
    APUState* getAPUState();

private:
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

    uint8_t NR50mem;
    uint8_t readyToPlay;
    int accumulated_cycles;
    uint8_t state;

    uint8_t period_envelope_a;
    uint8_t volume_envelope_a;
    uint8_t length_counter_a;
    uint8_t sweep_counter;
    uint16_t sweep_shadow_register;

    uint8_t period_envelope_b;
    uint8_t volume_envelope_b;
    uint8_t length_counter_b;

    uint16_t length_counter_wave;

    uint8_t period_envelope_noise;
    uint8_t volume_envelope_noise;
    uint16_t length_counter_noise;

    const float WAVE_VOLUMES[4] = {0.0f, 0.25f, 0.125f, 0.0675f};
};


#endif //LAME_BOY_APU_H

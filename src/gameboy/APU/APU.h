#pragma once

//Register addresses
#define NR10_ADDRESS 0xFF10
#define NR11_ADDRESS 0xFF11
#define NR12_ADDRESS 0xFF12
#define NR13_ADDRESS 0xFF13
#define NR14_ADDRESS 0xFF14

#define NR20_ADDRESS 0xFF15
#define NR21_ADDRESS 0xFF16
#define NR22_ADDRESS 0xFF17
#define NR23_ADDRESS 0xFF18
#define NR24_ADDRESS 0xFF19

#define NR30_ADDRESS 0xFF1A
#define NR31_ADDRESS 0xFF1B
#define NR32_ADDRESS 0xFF1C
#define NR33_ADDRESS 0xFF1D
#define NR34_ADDRESS 0xFF1E

#define NR40_ADDRESS 0xFF1F
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
#include "APUState.h"
#include "IVolumeController.h"

/**
 * This class emulates the functionality of the Game Boy APU.
 * Reading and writing to the APU registers is managed by this class and events are triggered according to the
 * documentation.
 * The state of the APU can be retrieved in order to mimic the audio output, normally handled by a DAC and mixer.
 */
class APU {
public:
    explicit APU();

    /**
     * Reads register value from address 0xff10 to address 0xff3f
     * @param address to register
     * @return register value if address is between 0xff10 and 0xff3f, 0xFF otherwise
     */
    uint8_t read(uint16_t address) const;

    /**
     * Writes to register from address 0xff10 to address 0xff3f, any address outside of this will be ignored
     * @param address to register
     * @param data is the value which will be written
     */
    void write(uint16_t address, uint8_t data);

    /**
     * Resets all registers to their default values
     */
    void reset();

    /**
     * Updates the APU timers and trigger events accordingly
     * @param cpuCycles the number of CPU cycles which has passed
     * @param vc is the volume controller which can change the volume of specific sources
     */
    void update(uint16_t cpuCycles, IVolumeController* vc);

    /**
     * Indicates if the status of one or more channels has changed
     * @return bit x == 1 -> source x has changed and the output should change
     */
    uint8_t isReadyToPlaySound();

    /**
     * Resets readyToPlay
     */
    void confirmPlay();

    /**
     * @return the state of all audio channels, and the waveform of the wave channel
     */
    APUState* getAPUState();

private:
    uint8_t NR10{};
    uint8_t NR11{};
    uint8_t NR12{};
    uint8_t NR13{};
    uint8_t NR14{};

    uint8_t NR21{};
    uint8_t NR22{};
    uint8_t NR23{};
    uint8_t NR24{};

    uint8_t NR30{};
    uint8_t NR31{};
    uint8_t NR32{};
    uint8_t NR33{};
    uint8_t NR34{};
    std::array<uint8_t, 16> wavePatternRAM{};

    uint8_t NR41{};
    uint8_t NR42{};
    uint8_t NR43{};
    uint8_t NR44{};

    uint8_t NR50{};
    uint8_t NR51{};
    uint8_t NR52;

    uint8_t readyToPlay;
    int accumulatedCycles;
    uint8_t state;

    uint8_t periodEnvelopeA{};
    uint8_t volumeEnvelopeA;
    uint8_t lengthCounterA{};
    uint8_t sweepCounter{};
    uint16_t sweepShadowRegister{};
    uint8_t sweepEnabled{};

    uint8_t periodEnvelopeB{};
    uint8_t volumeEnvelopeB;
    uint8_t lengthCounterB{};

    uint16_t lengthCounterWave{};

    uint8_t periodEnvelopeNoise{};
    uint8_t volumeEnvelopeNoise{};
    uint16_t lengthCounterNoise{};

    const float WAVE_VOLUMES[4] = {0.0f, 0.25f, 0.125f, 0.0675f};

    void volumeReset(uint8_t source);
    void sweepReset();
    uint16_t calculateSweep();

    void triggerEvent(uint8_t source);
    void lengthStep();
    void volEnvelopeStep(IVolumeController* vc);
    void sweepStep();
};



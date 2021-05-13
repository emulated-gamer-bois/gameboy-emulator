#include "APU.h"

APU::APU() {
    accumulatedCycles = 0;
    readyToPlay = 0;
    accumulatedCycles = 0;
    state = 0;
    volumeEnvelopeA = 0;
    volumeEnvelopeB = 0;
    reset();
    NR52 = 0;
}

uint8_t APU::read(uint16_t address) const {
    if (WAVE_PATTERN_START <= address && address <= WAVE_PATTERN_END) {
        return wavePatternRAM[address - WAVE_PATTERN_START];
    }
    switch (address) {
        case NR10_ADDRESS:
            return NR10 | 0x80;
        case NR11_ADDRESS:
            return NR11 | 0x3F;
        case NR12_ADDRESS:
            return NR12;
        case NR13_ADDRESS:
            return 0xFF;
        case NR14_ADDRESS:
            return NR14 | 0xBF;
        case NR20_ADDRESS:
            return 0xFF;
        case NR21_ADDRESS:
            return NR21 | 0x3F;
        case NR22_ADDRESS:
            return NR22;
        case NR23_ADDRESS:
            return 0xFF;
        case NR24_ADDRESS:
            return NR24 | 0xBF;
        case NR30_ADDRESS:
            return NR30 | 0x7F;
        case NR31_ADDRESS:
            return 0xFF;
        case NR32_ADDRESS:
            return NR32 | 0x9F;
        case NR33_ADDRESS:
            return 0xFF;
        case NR34_ADDRESS:
            return NR34 | 0xBF;
        case NR40_ADDRESS:
            return 0xFF;
        case NR41_ADDRESS:
            return 0xFF;
        case NR42_ADDRESS:
            return NR42;
        case NR43_ADDRESS:
            return NR43;
        case NR44_ADDRESS:
            return NR43 | 0xBF;
        case NR50_ADDRESS:
            return NR50;
        case NR51_ADDRESS:
            return NR51;
        case NR52_ADDRESS:
             return NR52 |                  //Power status at bit 7
             ((NR44 >> 4) & 8) |            //Noise status at bit 3
             (((NR34 & NR30) >> 5) & 4) |   //Wave status at bit 2
             ((NR24 >> 6) & 2) |            //Square 2 status at bit 1
             (NR14 >> 7) |                  //Square 1 status at bit 0
             0x70;                          //Bit 4 to 6 always 1
        default:
            return 0xFF;
    }
}

void APU::write(uint16_t address, uint8_t data) {
    if (WAVE_PATTERN_START <= address && address <= WAVE_PATTERN_END) {
        wavePatternRAM[address - WAVE_PATTERN_START] = data;
        return;
    }
    switch (address) {
        case NR10_ADDRESS:
            NR10 = data;
            sweepReset();
            readyToPlay |= 1;
            return;
        case NR11_ADDRESS:
            NR11 = data;
            return;
        case NR12_ADDRESS:
            NR12 = data;
            volumeReset(0);
            readyToPlay |= 1;
            return;
        case NR13_ADDRESS:
            NR13 = data;
            return;
        case NR14_ADDRESS:
            NR14 = data;
            // If trigger bit is set
            if(NR14 & 0x80) {
                triggerEvent(0);
            }
            return;
        case NR21_ADDRESS:
            NR21 = data;
            return;
        case NR22_ADDRESS:
            NR22 = data;
            volumeReset(1);
            readyToPlay |= 2;
            return;
        case NR23_ADDRESS:
            NR23 = data;
            return;
        case NR24_ADDRESS:
            NR24 = data;
            // If trigger bit is set
            if(NR24 & 0x80) {
                triggerEvent(1);
            }
            return;
        case NR30_ADDRESS:
            NR30 = data;
            readyToPlay |= 4;
            return;
        case NR31_ADDRESS:
            NR31 = data;
            return;
        case NR32_ADDRESS:
            NR32 = data;
            volumeReset(2);
            readyToPlay |= 4;
            return;
        case NR33_ADDRESS:
            NR33 = data;
            return;
        case NR34_ADDRESS:
            NR34 = data;
            // If trigger bit is set
            if(NR34 & 0x80) {
                triggerEvent(2);
            }
            return;
        case NR41_ADDRESS:
            NR41 = data;
            return;
        case NR42_ADDRESS:
            NR42 = data;
            volumeReset(3);
            readyToPlay |= 8;
            return;
        case NR43_ADDRESS:
            NR43 = data;
            return;
        case NR44_ADDRESS:
            NR44 = data;
            // If trigger bit is set
            if(NR44 & 0x80) {
                triggerEvent(3);
            }
            return;
        case NR50_ADDRESS:
            NR50 = data;
            return;
        case NR51_ADDRESS:
            NR51 = data;
            return;
        case NR52_ADDRESS:
            NR52 = data & 0x80;
            reset();
            if(!(NR52 & 0x80)) {
                readyToPlay |= 0xF;
            }
            return;
        default:
            return;
    }
}

void APU::reset() {
    NR10 = 0;
    NR11 = 0;
    NR12 = 0;
    NR13 = 0;
    NR14 = 0;

    NR21 = 0;
    NR22 = 0;
    NR23 = 0;
    NR24 = 0;

    NR30 = 0;
    NR31 = 0;
    NR32 = 0;
    NR33 = 0;
    NR34 = 0;

    NR41 = 0;
    NR42 = 0;
    NR43 = 0;
    NR44 = 0;

    NR50 = 0;
    NR51 = 0;

    wavePatternRAM = {0x00, 0xff, 0x00, 0xff,
                      0x00, 0xff, 0x00, 0xff,
                      0x00, 0xff, 0x00, 0xff,
                      0x00, 0xff, 0x00, 0xff};
}

void APU::volumeReset(uint8_t source) {
    switch (source) {
        case 0:
            periodEnvelopeA = NR12 & 0x7;
            volumeEnvelopeA = (NR12 >> 4) & 0xF;
            break;
        case 1:
            periodEnvelopeB = NR22 & 0x7;
            volumeEnvelopeB = (NR22 >> 4) & 0xF;
            break;
        case 2:
            break;
        case 3:
            periodEnvelopeNoise = NR42 & 0x7;
            volumeEnvelopeNoise = (NR42 >> 4) & 0xF;
            break;
    }
}

void APU::sweepReset() {
    sweepCounter = (NR10 >> 4) & 7;
    sweepShadowRegister = ((NR14 & 7) << 8) + NR13;

    sweepEnabled = (NR10 & 0x7) || (NR10 & 0x70);
    if((NR10 & 0x7) && calculateSweep() > 0x7FF) {
        sweepEnabled = false;
        NR14 &= 0x7F;
    }
}

uint16_t APU::calculateSweep() {
    if(NR10 & 8) {
        return sweepShadowRegister - sweepShadowRegister / (1 << (NR10 & 7));
    } else {
        return sweepShadowRegister + sweepShadowRegister / (1 << (NR10 & 7));
    }
}

void APU::triggerEvent(uint8_t source) {
    switch(source) {
        case 0:
            //If length counter is zero, it is set to 64
            lengthCounterA = NR11 & 0x3F ? NR11 & 0x3F : 0x40;
            sweepReset();
            readyToPlay |= 1;
            break;
        case 1:
            //If length counter is zero, it is set to 64
            lengthCounterB = NR21 & 0x3F ? NR21 & 0x3F : 0x40;
            readyToPlay |= 2;
            break;
        case 2:
            //If length counter is zero, it is set to 256
            lengthCounterWave = NR31 ? NR11 : 0x100;
            readyToPlay |= 4;
            break;
        case 3:
            //If length counter is zero, it is set to 64
            lengthCounterNoise = NR41 & 0x3F ? NR41 & 0x3F : 0x40;
            readyToPlay |= 8;
            break;
    }
    volumeReset(source);
}

void APU::lengthStep() {
    if((NR14 & 0x40) && lengthCounterA) {
        if(!--lengthCounterA) {
            NR14 &= 0x7F;
            readyToPlay |= 1;
        }
    }

    if((NR24 & 0x40) && lengthCounterB) {
        if(!--lengthCounterB) {
            NR24 &= 0x7F;
            readyToPlay |= 2;
        }
    }

    if((NR34 & 0x40) && lengthCounterWave) {
        if(!--lengthCounterWave) {
            NR34 &= 0x7F;
            readyToPlay |= 4;
        }
    }

    if((NR44 & 0x40) && lengthCounterNoise) {
        if(!--lengthCounterNoise) {
            NR44 &= 0x7F;
            readyToPlay |= 8;
        }
    }
}

void APU::volEnvelopeStep(IVolumeController* vc) {
    if((NR12 & 0x7) && !--periodEnvelopeA) {
        periodEnvelopeA = NR12 & 0x7;
        //If in increment mode and envelope can be incremented
        if((NR12 & 8) && (volumeEnvelopeA < 15)) {
            volumeEnvelopeA++;
        }
        //If in decrement mode and envelope can be decremented
        if(!(NR12 & 8) && volumeEnvelopeA) {
            volumeEnvelopeA--;
        }
        vc->setVolume(0, (float)volumeEnvelopeA / 15.0f);
    }

    if((NR22 & 0x7) && !--periodEnvelopeB) {
        periodEnvelopeB = NR22 & 0x7;
        //If in increment mode and envelope can be incremented
        if((NR22 & 8) && (volumeEnvelopeB < 15)) {
            volumeEnvelopeB++;
        }
        //If in decrement mode and envelope can be decremented
        if(!(NR22 & 8) && volumeEnvelopeB) {
            volumeEnvelopeB--;
        }
        vc->setVolume(1, (float)volumeEnvelopeB / 15.0f);
    }

    if((NR42 & 0x7) && !--periodEnvelopeNoise) {
        periodEnvelopeNoise = NR42 & 0x7;
        //If in increment mode and envelope can be incremented
        if((NR42 & 8) && (volumeEnvelopeNoise < 15)) {
            volumeEnvelopeNoise++;
        }
        //If in decrement mode and envelope can be decremented
        if(!(NR42 & 8) && volumeEnvelopeNoise) {
            volumeEnvelopeNoise--;
        }
        vc->setVolume(3, (float)volumeEnvelopeNoise / 15.0f);
    }
}

void APU::sweepStep() {
    if((NR10 & 0x70) && sweepEnabled && !--sweepCounter) {
        sweepShadowRegister = calculateSweep();
        if(sweepShadowRegister > 0x7FF) {
            NR14 &= 0x7F;
        }
        sweepCounter = (NR10 >> 4) & 7;
        readyToPlay |= 1;
    }
}

void APU::update(uint16_t cpuCycles, IVolumeController* vc) {
    accumulatedCycles += cpuCycles;
    if(accumulatedCycles < CLOCK_CYCLE_THRESHOLD) {
        return;
    }
    accumulatedCycles -= CLOCK_CYCLE_THRESHOLD;

    state++;
    state %= 8;

    if(state % 2 == 0) {
        lengthStep();
    }
    if(state == 7) {
        volEnvelopeStep(vc);
    }
    if(state % 4 == 2) {
        sweepStep();
    }
}
uint8_t APU::isReadyToPlaySound() {
    return readyToPlay;
}

void APU::confirmPlay() {
    readyToPlay = 0;
}

//Returns the state of the
APUState* APU::getAPUState() {
    return new APUState{
        (NR14 & 0x80) && (NR52 & 0x80),
        (uint8_t)((NR11 >> 6) & 0x3),
        sweepShadowRegister,
        (float)volumeEnvelopeA / 15.0f,

        (NR24 & 0x80) && (NR52 & 0x80),
        (uint8_t)((NR21 >> 6) & 0x3),
        (uint16_t)((((uint16_t)(NR24 & 0x7)) << 8) + NR23),
        (float)volumeEnvelopeB / 15.0f,

        (NR34 & 0x80) && (NR30 & 0x80) && (NR52 & 0x80),
        wavePatternRAM,
        (uint16_t)((((uint16_t)(NR34 & 0x7)) << 8) + NR33),
        WAVE_VOLUMES[(NR32 >> 5) & 0x3],

        (NR44 & 0x80) && (NR52 & 0x80),
        (int)((524288.0 / (NR43 & 0x07 ? NR43 & 0x07 : 0.5f)) / (2 << (NR43 >> 4))),
        (float)volumeEnvelopeNoise / 15.0f,
        (bool)(NR43 & 8)
    };
}

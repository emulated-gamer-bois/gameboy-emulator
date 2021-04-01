//
// Created by riddarvid on 3/24/21.
//

#include "APU.h"

APU::APU() {
    this->accumulated_cycles = 0;
    this->readyToPlay = 0;
    this->accumulated_cycles = 0;
    this->state = 0;
    this->volume_envelope_a = 0;
    this->volume_envelope_b = 0;
    this->reset();
}

uint8_t APU::read(uint16_t address) const {
    if (IO_WAVEFORM_RAM_START <= address && address <= IO_WAVEFORM_RAM_END) {
        return this->wavePatternRAM[address - IO_WAVEFORM_RAM_START];
    }
    switch (address) {
        case NR10_ADDRESS:
            return this->NR10;
        case NR11_ADDRESS:
            return this->NR11;
        case NR12_ADDRESS:
            return this->NR12;
        case NR13_ADDRESS:
            return this->NR13;
        case NR14_ADDRESS:
            return this->NR14;
        case NR21_ADDRESS:
            return this->NR21;
        case NR22_ADDRESS:
            return this->NR22;
        case NR23_ADDRESS:
            return this->NR23;
        case NR24_ADDRESS:
            return this->NR24;
        case NR50_ADDRESS:
            return this->NR50;
        case NR51_ADDRESS:
            return this->NR51;
        case NR52_ADDRESS:
            return this->NR52;
        default:
            //std::cout << "Tried to read from unimplemented audio address " << address << std::endl;
            return 0;
    }
}

void APU::write(uint16_t address, uint8_t data) {
    if (IO_WAVEFORM_RAM_START <= address && address <= IO_WAVEFORM_RAM_END) {
        this->wavePatternRAM[address - IO_WAVEFORM_RAM_START] = data;
    }
    switch (address) {
        case NR10_ADDRESS:
            this->NR10 = data;
            return;
        case NR11_ADDRESS:
            this->NR11 = data;
            return;
        case NR12_ADDRESS:
            this->NR12 = data;
            return;
        case NR13_ADDRESS:
            this->NR13 = data;
            return;
        case NR14_ADDRESS:
            this->NR14 = data;
            // If trigger bit is set
            if(this->NR14 & 0x80) {
                trigger_event(0);
            }
            return;
        case NR21_ADDRESS:
            this->NR21 = data;
            return;
        case NR22_ADDRESS:
            this->NR22 = data;
            return;
        case NR23_ADDRESS:
            this->NR23 = data;
            return;
        case NR24_ADDRESS:
            this->NR24 = data;
            // If trigger bit is set
            if(this->NR24 & 0x80) {
                trigger_event(1);
            }
            return;
        case NR30_ADDRESS:
            this->NR30 = data;
            this->readyToPlay |= 4;
            return;
        case NR31_ADDRESS:
            this->NR31 = data;
            return;
        case NR32_ADDRESS:
            this->NR32 = data;
            return;
        case NR33_ADDRESS:
            this->NR33 = data;
            return;
        case NR34_ADDRESS:
            this->NR34 = data;
            // If trigger bit is set
            if(this->NR34 & 0x80) {
                trigger_event(2);
            }
            return;
        case NR50_ADDRESS:
            this->NR50 = data;
            return;
        case NR51_ADDRESS:
            this->NR51 = data;
            return;
        case NR52_ADDRESS:
            this->NR52 = data;
            return;
        default:
            if(address >= WAVE_PATTERN_START && address <= WAVE_PATTERN_END){
                wavePatternRAM[address - WAVE_PATTERN_START] = data;
                return;
            }
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

    wavePatternRAM.fill(0);

    NR41 = 0;
    NR42 = 0;
    NR43 = 0;
    NR44 = 0;

    NR50 = 0;
    NR51 = 0;
    NR52 = 0;
}

void APU::trigger_event(uint8_t source) {
    //TODO: Complete functionality
    /**
    - Done - Channel is enabled (see length counter).
    - Done - If length counter is zero, it is set to 64 (256 for wave channel).
    - Done-ish (since frequency timer does not change in our program, it does not need to be reset)
        - Frequency timer is reloaded with period.
    - Done - Volume envelope timer is reloaded with period.
    - Done - Channel volume is reloaded from NRx2.
    - Pass - Noise channel's LFSR bits are all set to 1.
    - Pass - Wave channel's position is set to 0 but sample buffer is NOT refilled.
    - Pass - Square 1's sweep does several things (see frequency sweep).
     */
    switch(source) {
        case 0:
            //If length counter is zero, it is set to 64
            length_counter_a = this->NR11 & 0x3F ? this->NR11 & 0x3F : 0x40;

            this->period_envelope_a = this->NR12 & 0x7;
            this->volume_envelope_a = (this->NR12 >> 4) & 0xF;
            this->readyToPlay |= 1;
            break;
        case 1:
            //If length counter is zero, it is set to 64
            length_counter_b = this->NR21 & 0x3F ? this->NR21 & 0x3F : 0x40;
            this->period_envelope_b = this->NR22 & 0x7;
            this->volume_envelope_b = (this->NR22 >> 4) & 0xF;
            this->readyToPlay |= 2;
            break;
        case 2:
            //If length counter is zero, it is set to 256
            length_counter_wave = this->NR31 ? this->NR11 : 0x100;
            this->readyToPlay |= 4;
            break;
    }
}

void APU::length_step() {
    if((this->NR14 & 0x40) && length_counter_a) {
        if(!--this->length_counter_a) {
            this->NR14 &= 0x7F;
            readyToPlay |= 1;
        }
    }

    if((this->NR24 & 0x40) && length_counter_b) {
        if(!--this->length_counter_b) {
            this->NR24 &= 0x7F;
            readyToPlay |= 2;
        }
    }

    if((this->NR34 & 0x40) && length_counter_wave) {
        if(!--this->length_counter_wave) {
            this->NR34 &= 0x7F;
            readyToPlay |= 4;
        }
    }
}

void APU::vol_envelope_step(IVolumeController* vc) {
    if((this->NR12 & 0x7) && !--this->period_envelope_a) {
        this->period_envelope_a = this->NR12 & 0x7;
        //If in increment mode and envelope can be incremented
        if((this->NR12 & 8) && (volume_envelope_a < 15)) {
            volume_envelope_a++;
        }
        //If in decrement mode and envelope can be decremented
        if(!(this->NR12 & 8) && volume_envelope_a) {
            volume_envelope_a--;
        }
        vc->setVolume(0, (float)volume_envelope_a/15.0f);
    }

    if((this->NR22 & 0x7) && !--this->period_envelope_b) {
        this->period_envelope_b = this->NR22 & 0x7;
        //If in increment mode and envelope can be incremented
        if((this->NR22 & 8) && (volume_envelope_b < 15)) {
            volume_envelope_b++;
        }
        //If in decrement mode and envelope can be decremented
        if(!(this->NR22 & 8) && volume_envelope_b) {
            volume_envelope_b--;
        }
        vc->setVolume(1, (float)volume_envelope_b/15.0f);
    }
}

void APU::sweep_step() {
    //TODO: Update with sweep
}

void APU::update(uint16_t cpuCycles, IVolumeController* vc) {
    this->accumulated_cycles += cpuCycles;
    if(this->accumulated_cycles < CLOCK_CYCLE_THRESHOLD) {
        return;
    }
    this->accumulated_cycles -= CLOCK_CYCLE_THRESHOLD;

    this->state++;
    state %= 8;

    if(state % 2 == 0) {
        this->length_step();
    }
    if(state == 7) {
        this->vol_envelope_step(vc);
    }
    if(state % 4 == 2) {
        this->sweep_step();
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
        (bool)(this->NR14 & 0x80),
        (uint8_t)((this->NR11 >> 6) & 0x3),
        (uint16_t)((((uint16_t)(this->NR14 & 0x7)) << 8) + this->NR13),
        (float)volume_envelope_a / 15.0f,

        (bool)(this->NR24 & 0x80),
        (uint8_t)((this->NR21 >> 6) & 0x3),
        (uint16_t)((((uint16_t)(this->NR24 & 0x7)) << 8) + this->NR23),
        (float)volume_envelope_b / 15.0f,

        (this->NR34 & 0x80) && (this->NR30 & 0x80),
        wavePatternRAM,
        (uint16_t)((((uint16_t)(this->NR34 & 0x7)) << 8) + this->NR33),
        WAVE_VOLUMES[(this->NR32 >> 5) & 0x3],
    };
}
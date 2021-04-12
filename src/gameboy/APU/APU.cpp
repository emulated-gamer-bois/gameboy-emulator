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
    wavePatternRAM.fill(0);
}

uint8_t APU::read(uint16_t address) const {
    if (IO_WAVEFORM_RAM_START <= address && address <= IO_WAVEFORM_RAM_END) {
        return this->wavePatternRAM[address - IO_WAVEFORM_RAM_START];
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
            return this->NR50;
        case NR51_ADDRESS:
            return this->NR51;
        case NR52_ADDRESS:
             return NR52 |                  //Power status at bit 7
             ((NR44 >> 4) & 8) |            //Noise status at bit 3
             (((NR34 & NR30) >> 5) & 4) |   //Wave status at bit 2
             ((NR24 >> 6) & 2) |            //Square 2 status at bit 1
             (NR14 >> 7) |                  //Square 1 status at bit 0
             0x70;                          //Bit 4 to 6 always 1
        default:
            return 0;
    }
}

void APU::write(uint16_t address, uint8_t data) {
    if(!(NR52 & 0x80)) {
        switch (address) {
            case NR11_ADDRESS:
            case NR21_ADDRESS:
            case NR31_ADDRESS:
            case NR41_ADDRESS:
            case NR52_ADDRESS:
                break;
            default:
                //Ignoring any writes while APU is off
                //Length registers can still be written to while power is off
                return;
        }
    }
    if (IO_WAVEFORM_RAM_START <= address && address <= IO_WAVEFORM_RAM_END) {
        this->wavePatternRAM[address - IO_WAVEFORM_RAM_START] = data;
        //return;
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
        case NR41_ADDRESS:
            this->NR41 = data;
            return;
        case NR42_ADDRESS:
            this->NR42 = data;
            return;
        case NR43_ADDRESS:
            this->NR43 = data;
            return;
        case NR44_ADDRESS:
            this->NR44 = data;
            // If trigger bit is set
            if(this->NR44 & 0x80) {
                trigger_event(3);
            }
            return;
        case NR50_ADDRESS:
            this->NR50 = data;
            return;
        case NR51_ADDRESS:
            this->NR51 = data;
            return;
        case NR52_ADDRESS:
            this->NR52 = data & 0x80;
            if(!(this->NR52 & 0x80)) {
                reset();
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
    - Done-ish (done in audio controller) - Noise channel's LFSR bits are all set to 1.
    - Pass - Wave channel's position is set to 0 but sample buffer is NOT refilled.
    - Pass - Square 1's sweep does several things (see frequency sweep).
     */
    switch(source) {
        case 0:
            //If length counter is zero, it is set to 64
            length_counter_a = this->NR11 & 0x3F ? this->NR11 & 0x3F : 0x40;

            this->period_envelope_a = this->NR12 & 0x7;
            this->volume_envelope_a = (this->NR12 >> 4) & 0xF;

            //Sweep
            sweep_counter = (NR10 >> 4) & 7;
            sweep_shadow_register = ((NR14 & 7) << 8) + NR13;

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
        case 3:
            //If length counter is zero, it is set to 64
            length_counter_noise = this->NR41 & 0x3F ? this->NR41 & 0x3F : 0x40;
            this->period_envelope_noise = this->NR42 & 0x7;
            volume_envelope_noise = (this->NR42 >> 4) & 0xF;
            this->readyToPlay |= 8;
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

    if((this->NR44 & 0x40) && length_counter_noise) {
        if(!--this->length_counter_noise) {
            this->NR44 &= 0x7F;
            readyToPlay |= 8;
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

    if((this->NR42 & 0x7) && !--this->period_envelope_noise) {
        this->period_envelope_noise = this->NR42 & 0x7;
        //If in increment mode and envelope can be incremented
        if((this->NR42 & 8) && (volume_envelope_noise < 15)) {
            volume_envelope_noise++;
        }
        //If in decrement mode and envelope can be decremented
        if(!(this->NR42 & 8) && volume_envelope_noise) {
            volume_envelope_noise--;
        }
        vc->setVolume(3, (float)volume_envelope_noise/15.0f);
    }
}

void APU::sweep_step() {
    if((NR10 & 0x70) && !--sweep_counter) {
        if(NR10 & 8) {
            sweep_shadow_register -= sweep_shadow_register/(1 << (NR10 & 7));
        } else {
            sweep_shadow_register += sweep_shadow_register/(1 << (NR10 & 7));
        }
        sweep_counter = (NR10 >> 4) & 7;
        readyToPlay |= 1;
    }
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
        (NR14 & 0x80) && (NR52 & 0x80),
        (uint8_t)((this->NR11 >> 6) & 0x3),
        sweep_shadow_register,
        (float)volume_envelope_a / 15.0f,

        (NR24 & 0x80) && (NR52 & 0x80),
        (uint8_t)((this->NR21 >> 6) & 0x3),
        (uint16_t)((((uint16_t)(this->NR24 & 0x7)) << 8) + this->NR23),
        (float)volume_envelope_b / 15.0f,

        (NR34 & 0x80) && (NR30 & 0x80) && (NR52 & 0x80),
        wavePatternRAM,
        (uint16_t)((((uint16_t)(this->NR34 & 0x7)) << 8) + this->NR33),
        WAVE_VOLUMES[(this->NR32 >> 5) & 0x3],

        (NR44 & 0x80) && (NR52 & 0x80),
        (int)((524288.0 / (NR43 & 0x07 ? NR43 & 0x07 : 0.5f)) / (2 << (NR43 >> 4))),
        (float)volume_envelope_noise/15.0f,
        (bool)(this->NR43 & 8)
    };
}

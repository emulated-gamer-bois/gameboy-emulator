//
// Created by David on 2021-03-27.
//

#pragma once

struct APUState {
    bool enable_square_a;
    uint8_t duty_square_a;
    uint16_t frequency_square_a;
    float volume_square_a;

    bool enable_square_b;
    uint8_t duty_square_b;
    uint16_t frequency_square_b;
    float volume_square_b;

    bool enable_wave;
    std::array<uint8_t, 16> waveform_wave;
    uint16_t frequency_wave;
    float volume_wave;

    bool enable_noise;
    int frequency_noise;
    float volume_noise;
    bool is_7_bit_mode;
};

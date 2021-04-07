//
// Created by David on 2021-03-26.
//

#include <chrono>
#include <thread>
#include <array>
#include <cmath>
#include "gtest/gtest.h"
#include "../src/IO/AudioController.h"

TEST(AUDIO, START_SOUND) {
    AudioController a;
    a.playGBSquare(0, 2, 0x783, 0.1f);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    a.playGBSquare(0, 2, 0x7C1, 0.1f);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    a.stopSound();
}

TEST(AUDIO, SAWTOOTH_WAVE) {
    std::array<uint8_t, 16> sawTooth{};
    for(auto i = 0; i < 16; i++) {
        sawTooth[i] =  ((0xF - i) << 4) + (0xF - i);
    }

    AudioController a;
    a.playGBWave(0, sawTooth, 0x6D6, 0.1f);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    a.stopSound();
}

TEST(AUDIO, SIN_WAVE) {
    std::array<uint8_t, 16> sine{};
    for(auto i = 0; i < 16; i++) {
        sine[i] =  (((uint8_t)(8 + 8 * sin(3.141592 * 2 * ((double)i*2/32)))*16) & 0xF0) +
                ((uint8_t)(8 + 8 * sin(3.141592 * 2 * ((double)(i*2+1)/32))) & 0xF);
    }

    AudioController a;
    a.playGBWave(0, sine, 0x6D6, 0.1f);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    a.stopSound();
}

TEST(AUDIO, START_SOUND_B) {
    AudioController a;
    a.playGBSquare(1, 2, 0x783, 0.1f);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    a.playGBSquare(1, 2, 0x7C1, 0.1f);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    a.stopSound();
}

TEST(AUDIO, NOISE) {
    const auto size = 10000;
    unsigned char samples[size];
    uint16_t lfsr = 0x7F;
    auto xor_res = 0;
    auto sample = 0;
    for(auto i = 0; i < size; i++) {
        xor_res = (lfsr ^ (lfsr >> 1)) & 1;
        lfsr >>= 1;
        lfsr &= 0xFFBF;
        lfsr |= /*(xor_res << 14) |*/ (xor_res << 6);
        sample = lfsr & 1;
        //std::cout << std::hex << lfsr << ";" << sample << ";" << xor_res << "\n";
        samples[i] = sample ? 0 : (char)0xFF;
        //std::cout << (int)samples[i] << "\n";
    }

    AudioController a;
    a.playSound(3, samples, size, 10000, 0.1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    a.stopSound();
}

TEST(AUDIO, PLAY_NOISE) {
    AudioController a;
    uint8_t divisors[8] = {8,16,32,48,64,80,96,112};
    for(int i = 0; i < 8; i++){
        a.playNoise(3, 0, (262144*2)/divisors[i], 0.1);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    for(int i = 0; i < 8; i++){
        a.playNoise(3, 1, (262144*2)/divisors[i], 0.1);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    a.stopSound();
}

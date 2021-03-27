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
    a.init();
    a.playGBSquare(0, 2, 0x783, 1.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    a.stopSource(0);
    a.playGBSquare(0, 2, 0x7C1, 1.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    a.stopSource(0);
}

TEST(AUDIO, SAWTOOTH_WAVE) {
    std::array<uint8_t, 16> sawTooth{};
    for(auto i = 0; i < 16; i++) {
        sawTooth[i] =  ((0xF - i) << 4) + (0xF - i);
    }

    AudioController a;
    a.init();
    a.playGBWave(0, sawTooth, 0x6D6);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    a.stopSource(0);
}

TEST(AUDIO, SIN_WAVE) {
    std::array<uint8_t, 16> sine{};
    for(auto i = 0; i < 16; i++) {
        sine[i] =  (((uint8_t)(8 + 8 * sin(3.141592 * 2 * ((double)i*2/32)))*16) & 0xF0) +
                ((uint8_t)(8 + 8 * sin(3.141592 * 2 * ((double)(i*2+1)/32))) & 0xF);
    }

    AudioController a;
    a.init();
    a.playGBWave(0, sine, 0x6D6);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    a.stopSource(0);
}

TEST(AUDIO, START_SOUND_B) {
    AudioController a;
    a.init();
    a.playGBSquare(1, 2, 0x783, 1.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    a.stopSource(1);
    a.playGBSquare(1, 2, 0x7C1, 1.0f);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    a.stopSource(1);
}

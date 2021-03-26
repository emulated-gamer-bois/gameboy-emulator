//
// Created by David on 2021-03-26.
//

#include <chrono>
#include <thread>
#include "gtest/gtest.h"
#include "../src/IO/AudioController.h"

TEST(AUDIO, START_SOUND) {
    AudioController a;
    a.init();
    a.playGBSquare(0, 2, 0x783);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    a.stopSource(0);
    a.playGBSquare(0, 2, 0x7C1);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    a.stopSource(0);
}
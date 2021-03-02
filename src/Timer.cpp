#include "Timer.h"

void Timer::tick() {
    this->timeTicked = std::chrono::system_clock::now();
}

float Timer::msSinceTick() const {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<float, std::milli> elapsedTime = now - this->timeTicked;

    return elapsedTime.count();
}
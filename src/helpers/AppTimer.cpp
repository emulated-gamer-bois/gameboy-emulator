#include "AppTimer.h"

void AppTimer::tick() {
    this->timeTicked = std::chrono::system_clock::now();
}

float AppTimer::msSinceTick() const {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<float, std::milli> elapsedTime = now - this->timeTicked;

    return elapsedTime.count();
}
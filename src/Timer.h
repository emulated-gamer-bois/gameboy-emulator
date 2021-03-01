#pragma once

#include <chrono>

class Timer {
public:
    void tick();
    float msSinceTick() const;

private:
    std::chrono::time_point<std::chrono::system_clock> timeTicked;
};

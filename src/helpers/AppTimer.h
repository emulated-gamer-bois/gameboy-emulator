#pragma once

#include <chrono>

class AppTimer {
public:
    void tick();
    [[nodiscard]] float msSinceTick() const;

private:
    std::chrono::time_point<std::chrono::system_clock> timeTicked;
};

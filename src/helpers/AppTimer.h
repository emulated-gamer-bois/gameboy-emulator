#pragma once

#include <chrono>

class AppTimer {
public:
    /**
     * Starts a stopwatch.
     * */
    void tick();
    /**
     * Used to retrieve the timer since the stopwatch was started.
     * @return returns the time since the stopwatch was started, in milliseconds.
     * */
    [[nodiscard]] float msSinceTick() const;

private:
    std::chrono::time_point<std::chrono::system_clock> timeTicked;
};

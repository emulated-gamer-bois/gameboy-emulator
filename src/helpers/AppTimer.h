/**
 * A simple helper class for keeping how much system clock time has elapsed. Measured in milliseconds.
 */

#pragma once

#include <chrono>

class AppTimer {
public:
    /**
     * Starts or resets the timer.
     */
    void tick();

    /**
     * Used to retrieve the timer since the stopwatch was started.
     *
     * @return returns the time since the stopwatch was started, in milliseconds.
     */
    [[nodiscard]] float msSinceTick() const;

private:
    std::chrono::time_point<std::chrono::system_clock> timeTicked;
};

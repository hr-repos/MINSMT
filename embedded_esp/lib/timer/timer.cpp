#include "timer.h"

/// @brief Constructs a Timer object with a specified delay in milliseconds
/// @param delayInMs The delay duration in milliseconds
Timer::Timer(int delayInMs) : delaysms(delayInMs), lastCheckTime(0) {
    // Constructor implementation (if needed)
}

/// @brief Checks if the timer has elapsed
/// @return true if the timer has elapsed, false otherwise
bool Timer::checkTimer() {
    unsigned long currentTime = millis();
    if (currentTime - lastCheckTime >= delaysms) {
        lastCheckTime = currentTime;
        return true;
    }
    return false;
}

/// @brief Resets the timer to start counting from the current time
void Timer::resetTimer() {
    this->lastCheckTime = millis();
}
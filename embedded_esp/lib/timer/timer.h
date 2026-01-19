#ifndef TIMER_h
#define TIMER_h

#include <Arduino.h>

/// @brief Simple Timer class for managing non-blocking delays
class Timer
{
    private:
        int delaysms;
        unsigned long lastCheckTime;
    public:
        Timer(int delayInMs);
        bool checkTimer();
};

#endif

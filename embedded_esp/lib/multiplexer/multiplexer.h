#ifndef LIB_MULTIPLEXER_H_
#define LIB_MULTIPLEXER_H_

// Enter dependencies here
#include <Arduino.h>

#define MULTIPLEXER_CHANNELS_COUNT 16

class Multiplexer 
{
    private:
        int s0_pin;
        int s1_pin;
        int s2_pin;
        int s3_pin;
        int signal_pin;
    public:
        Multiplexer(int s0, int s1, int s2, int s3, int signal); // Constructor
        bool readChannel(int channel);
        bool* readAsArray(bool* array);
};

// | S0  | S1  | S2  | S3  | selected channel |
// | --- | --- | --- | --- | ---------------- |
// | 0   | 0   | 0   | 0   | 0                |
// | 1   | 0   | 0   | 0   | 1                |
// | 0   | 1   | 0   | 0   | 2                |
// | 1   | 1   | 0   | 0   | 3                |
// | 0   | 0   | 1   | 0   | 4                |
// | 1   | 0   | 1   | 0   | 5                |
// | 0   | 1   | 1   | 0   | 6                |
// | 1   | 1   | 1   | 0   | 7                |
// | 0   | 0   | 0   | 1   | 8                |
// | 1   | 0   | 0   | 1   | 9                |
// | 0   | 1   | 0   | 1   | 10               |
// | 1   | 1   | 0   | 1   | 11               |
// | 0   | 0   | 1   | 1   | 12               |
// | 1   | 0   | 1   | 1   | 13               |
// | 0   | 1   | 1   | 1   | 14               |
// | 1   | 1   | 1   | 1   | 15               |

#endif  // LIB_MULTIPLEXER_H_

#ifndef LIB_TESTLIB_H_
#define LIB_TESTLIB_H_

// Enter dependencies here
// #include <Arduino.h>

#define ENABLE_GxEPD2_GFX 0

// E-paper pinout
// EPD_PWR   7 - 3.3V out
// BUSY_PIN 21 - pico pin 27
// RES_PIN  20 - pico pin 26
// DC_PIN   16 - pico pin 21
// CS_PIN   17 - pico pin 22
// SCL      18 - pico pin 24
// SDA      19 - pico pin 25
// GND         - pico pin 38
// VCC         - pico pin 36

class TestLib {
    public:
        TestLib(); // Constructor
        void begin();
        int add(int a, int b);
};


#endif  // LIB_TESTLIB_H_

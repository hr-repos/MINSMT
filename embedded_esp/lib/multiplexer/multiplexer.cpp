# include "multiplexer.h"

/// @brief Constructor for the Multiplexer class
/// @param s0 Pin number for S0
/// @param s1 Pin number for S1
/// @param s2 Pin number for S2
/// @param s3 Pin number for S3
/// @param signal Pin number for the signal input
Multiplexer::Multiplexer(int s0, int s1, int s2, int s3, int signal) : 
    s0_pin(s0), s1_pin(s1), s2_pin(s2), s3_pin(s3), signal_pin(signal) 
{
    pinMode(s0_pin, OUTPUT);
    pinMode(s1_pin, OUTPUT);
    pinMode(s2_pin, OUTPUT);
    pinMode(s3_pin, OUTPUT);
    pinMode(signal_pin, INPUT_PULLUP);
}

/// @brief Reads the value from a specific channel of the multiplexer
/// @param channel The channel number to read (0-15)
/// @return The boolean value read from the channel
bool Multiplexer::readChannel(int channel) 
{
    // Set the select pins based on the channel number
    digitalWrite(s0_pin, (channel >> 0) & 0x01);
    digitalWrite(s1_pin, (channel >> 1) & 0x01);
    digitalWrite(s2_pin, (channel >> 2) & 0x01);
    digitalWrite(s3_pin, (channel >> 3) & 0x01);

    // Serial.print("Reading channel ");
    // Serial.print(channel);
    // Serial.print(": S0=");
    // Serial.print((channel >> 0) & 0x01);
    // Serial.print(" S1=");
    // Serial.print((channel >> 1) & 0x01);
    // Serial.print(" S2=");
    // Serial.print((channel >> 2) & 0x01);
    // Serial.print(" S3=");
    // Serial.print((channel >> 3) & 0x01);
    // Serial.print(" => Value=");
    // Serial.println(digitalRead(signal_pin));

    // Read and return the signal pin state
    return !digitalRead(signal_pin);
}

bool* Multiplexer::readAsArray(bool* array)
{
    for (int i = 0; i < MULTIPLEXER_CHANNELS_COUNT; i++) 
    {
        array[i] = readChannel(i);
    }
    return array;
}




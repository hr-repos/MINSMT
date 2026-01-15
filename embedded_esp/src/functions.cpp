// #include <Arduino.h>
#include "config.h"


/**
 * @brief Checks if the board is in the starting position
 * @param board A 2D array representing the board state
 * @return true if the board is in the starting position, false otherwise
 */
bool isStartPosition(bool board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]){
    for (int row; row < BOARDWIDTHHIGHT; row++)
    {
        for (int column; column < BOARDWIDTHHIGHT; column++) 
        {
            if (row == 0 || row == 1 || row == 6 || row == 7) 
            {
                // If the first two or last two rows are missing a piece, return false
                if(board[row][column] == false) 
                {
                    return false;
                } 
            } 
            else
            {
                // If any of the middle rows have a piece, return false
                if(board[row][column] == true) 
                {
                    return false;
                } 
            }
        }
    }

    // If all checks passed, the board is in the start position
    return true;
}

/**
 * @brief Callback function to handle incoming MQTT messages
 * @param topic The topic on which the message was received
 * @param payload The message payload
 * @param length The length of the payload
 */
void callback(char* topic, byte* payload, unsigned int length)
{
    char message[length + 1]; // +1 for null terminator
    for (int i = 0; i < length; i++) 
    {
        message[i] = (char)payload[i];
    }
    message[length] = '\0';
    // for debugging
    Serial.printf("Message arrived [%s] %s\n", topic, message);

    if (strcmp(topic, "topic1") == 0)
    {
        Serial.println("message received on topic1");
    }
}
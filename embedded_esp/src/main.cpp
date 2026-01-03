// Put external dependencies here:
#include <Arduino.h>

// Put internal dependencies here:
#include "multiplexer.h"
#define BOARDWIDTHHIGHT 8
#define MULTIPLEXERS_COUNT 4

// put function declarations here:
void readBoardState(bool* board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]);

// stores if a piece is detected on the corresponding place on the board
bool* boardPresence[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT] = {0};
// Contains multiplexer objects for all 4 multiplexers
Multiplexer* multiPlexers[MULTIPLEXERS_COUNT];

void setup()
{
    Serial.begin(115200);

    // Initialize multiplexers all sharing the same select pins. The signal pins should be unique
    multiPlexers[0] = new Multiplexer(19, 18, 5, 17, 16); // Mux for rows 1 and 2
    multiPlexers[1] = new Multiplexer(19, 18, 5, 17, 16); // Mux for rows 3 and 4
    multiPlexers[2] = new Multiplexer(19, 18, 5, 17, 16); // Mux for rows 5 and 6
    multiPlexers[3] = new Multiplexer(19, 18, 5, 17, 16); // Mux for rows 7 and 8

    // Met deze loop wacht de code tot de seriele monitor verbonden is voordat deze verder gaat, handig voor debuggen.
    // pinMode(LED_BUILTIN, OUTPUT);
    // digitalWrite(LED_BUILTIN, HIGH);
    // while (!Serial)
    // {
    //     digitalWrite(LED_BUILTIN, HIGH);
    //     delay(1000);
    //     digitalWrite(LED_BUILTIN, LOW);
    //     delay(1000);
    // }

    readBoardState(boardPresence);
}

void loop()
{
    // put your main code here, to run repeatedly:
}



/// @brief Reads the state of the entire board and updates the boardPresence array
/// @param board A 2D array representing the board state
/// @test Serial.printf("y: %d\tx: %d\tch: %d\n", h, w, channel);
void readBoardState(bool* board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT])
{
    for (int mPlexer = 0; mPlexer < MULTIPLEXERS_COUNT; mPlexer++)
    { 
        for (int channel = 0; channel < MULTIPLEXER_CHANNELS_COUNT; channel++)
        {
            int w = mPlexer * 2 + (channel / BOARDWIDTHHIGHT);
            int h = channel % BOARDWIDTHHIGHT;
            *board[w][h] = multiPlexers[mPlexer]->readChannel(channel);
        }
    }
}
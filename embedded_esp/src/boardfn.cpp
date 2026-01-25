#include "functions.h"
#include "config.h"
#include "logic.h"

/// @brief Reads the state of the entire board and updates the lastState array
/// @param board A 2D array representing the board state)
/// @test Serial.printf("y: %d\tx: %d\tch: %d\n", h, w, channel);
/// @note The board here is not a copy but a reference to the original array when compiled 
void readBoardState(bool board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT])
{
    for (int mPlexer = 0; mPlexer < MULTIPLEXERS_COUNT; mPlexer++)
    { 
        for (int channel = 0; channel < MULTIPLEXER_CHANNELS_COUNT; channel++)
        {
            int w = mPlexer * 2 + (channel / BOARDWIDTHHIGHT);
            int h = channel % BOARDWIDTHHIGHT; // 7- because the highest row of the board is 7
            board[w][h] = multiPlexers[mPlexer]->readChannel(channel);
        }
    }
}



// /// @brief Contains the main game loop for handling moves
// void moveLoop(){
//     String move = createMoveStr(/*before*/{}, /*after*/{});



//     delay(1000); // Placeholder for actual move handling logic
//     currentGameState = WAITING_FOR_OPPONENT_MOVE; // Placeholder to change game state
//     sendMove();
// }


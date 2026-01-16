#include "functions.h"
#include "config.h"

/// @brief Reads the state of the entire board and updates the boardPresence array
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
            int h = channel % BOARDWIDTHHIGHT;
            board[w][h] = false; //multiPlexers[mPlexer]->readChannel(channel);
            if (w == 3 && h == 5)
            {
                board[w][h] = true;
            }
        }
    }
}

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

/// @brief Contains the main game loop for handling moves
void moveLoop(){
    delay(10000); // Placeholder for actual move handling logic
    currentGameState = WAITING_FOR_OPPONENT_MOVE; // Placeholder to change game state
    sendMove();
}

/// @brief Generates a random play code which will be used as mqtt topic
/// @return The generated play code
int generatePlayCode(){
    return random(1000, 9999);
}
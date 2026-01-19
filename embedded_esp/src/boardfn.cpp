#include "functions.h"
#include "config.h"

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

/**
 * @brief Checks if the board is in the starting position
 * @param board A 2D array representing the board state
 * @return true if the board is in the starting position, false otherwise
 */
bool isStartPosition(bool board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]){
    for (int row = 0; row < BOARDWIDTHHIGHT; row++)
    {
        for (int column = 0; column < BOARDWIDTHHIGHT; column++) 
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
    String move = createMoveStr(/*before*/{}, /*after*/{});



    delay(1000); // Placeholder for actual move handling logic
    currentGameState = WAITING_FOR_OPPONENT_MOVE; // Placeholder to change game state
    sendMove();
}

/// @brief Generates a random play code which will be used as mqtt topic
/// @return The generated play code
int generatePlayCode(){
    return random(1000, 9999);
}

/// @brief This function creates a move string based on the board state before and after a move (eg. e2e4)
/// @param before 
/// @param after 
String createMoveStr(bool before [BOARDWIDTHHIGHT][BOARDWIDTHHIGHT], bool after [BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]){
    int fromY, toY;
    char fromX, toX;
    for (int row = 0; row < BOARDWIDTHHIGHT; row++)
    {
        for (int column = 0; column < BOARDWIDTHHIGHT; column++) 
        {
            if (before[row][column] != after[row][column]){
                if (before[row][column] == true){
                    // piece moved from this position
                    fromX = 'a' + column;
                    fromY = 8 - row;
                } else {
                    // piece moved to this position
                    toX = 'a' + column;
                    toY = 8 - row;
                }
            }
        }
    }
    return String(fromX) + String(fromY) + String(toX) + String(toY);
}

void moveStrToCoords(char* moveStr, int& fromX, int& fromY, int& toX, int& toY){
    // Example moveStr: "e2e4"
    fromX = moveStr[0] - 'a'; // Convert 'a'-'h' to 0-7
    fromY = moveStr[1] - '1'; // Convert '1'-'8' to 0-7
    toX = moveStr[2] - 'a';   // Convert 'a'-'h' to 0-7
    toY = moveStr[3] - '1';   // Convert '1'-'8' to 0-7
}
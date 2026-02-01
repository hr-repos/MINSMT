#include "functions.h"
#include "config.h"
#include "logic.h"

/// @brief Reads the state of the entire board and updates the lastState array
/// @param board A 2D array representing the board state)
/// @test Serial.printf("y: %d\tx: %d\tch: %d\n", h, w, channel);
/// @note The board here is not a copy but a reference to array the data will be stored in
void readBoardState(int muxCount, int channelCount, bool board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT])
{
    // Serial.println("Reading board state...");
    // Loop over all 4 multiplexers
    for (int mux = 0; mux < muxCount; mux++)
    {
        // Each multiplexer fills 2 rows
        int baseRow = mux * 2;

        // Loop over all 16 channels of the multiplexer
        for (int channel = 0; channel < channelCount; channel++)
        {
            // Determine which of the 2 rows this channel belongs to
            int rowOffset = channel / 8;   // 0 for channels 0–7, 1 for 8–15
            int row = baseRow + rowOffset;

            // Determine column inside the row
            int col = channel % 8;         // 0–7

            // Read the channel and store it in the board
            board[row][col] = multiPlexers[mux]->readChannel(channel);
            if (board[row][col]) {
                // Serial.printf("Piece detected at row %d, column %d (mux %d, channel %d)\n", row, col, mux, channel);
            }
        }
    }
}



/// @brief handles incoming move from opponent
void move(){
    // String move = createMoveStr(/*before*/{}, /*after*/{});



    delay(1000); // Placeholder for actual move handling logic
    currentGameState = WAITING_FOR_OPPONENT_MOVE; // Placeholder to change game state
}


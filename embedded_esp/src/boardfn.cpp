#include <iostream>
#include "functions.h"
#include "config.h"
#include "logic.h"

/// @brief Reads the state of the entire board and updates the lastMoveState array
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

int currentPiecesCount = 32;
bool firstPiecePickedUp[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]= {false};

/// @brief handles reading of the board movements to be sent to the opponent
void handleBoardMove(){
    bool currentState[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT] = {false};
    readBoardState(MULTIPLEXERS_COUNT, MULTIPLEXER_CHANNELS_COUNT, currentState);

    switch (currentMoveState)
    { 
        // detects picking up a piece
        case NO_MOVE: {
            auto change = detectChange(lastMoveState, currentState);

            if (change != std::make_pair(-1, -1)){
                std::cout << "Piece picked up at row " << change.first << ", column " << change.second << std::endl;
                currentMoveState = FIRST_MOVE_DETECTED;
                memcpy(firstPiecePickedUp, currentState, sizeof(firstPiecePickedUp));
            }
            break;
        }

        case FIRST_MOVE_DETECTED: {
            // if the amount of pieces is the same as before after a piece was picked up, it means 
            // the piece was placed without capturing
            if (countPieces(currentState) == currentPiecesCount){
                digitalWrite(pinLedBoardsTurn, LOW); // Turn off "board's turn" indicator
                
                std::string move = createMoveStr(lastMoveState,  firstPiecePickedUp, currentState);
                Serial.println("Detected move: " + String(move.c_str()));
                sendMessage((playcodeString + "/move/board").c_str(), move.c_str());

                memcpy(lastMoveState, currentState, sizeof(lastMoveState));
                currentGameState = WAITING_FOR_OPPONENT_MOVE;
                break;
            }
            // A second piece is picked up which indicated a capture
            // The correct location will be set manually here so the board state is up 
            // to date before sending the move to the dashboard
            if (countPieces(currentState) == currentPiecesCount - 2){
                std::string move = createMoveStr(lastMoveState, firstPiecePickedUp, currentState);
                std::cout << "Detected move with capture: " + move << std::endl;

                // This should not take long
                // this is blocking so the current state does not have the be stored in another variable
                // if this loop takes too long another case should be added to keep the system responsive
                while (countPieces(currentState) != currentPiecesCount - 1){
                    readBoardState(MULTIPLEXERS_COUNT, MULTIPLEXER_CHANNELS_COUNT, currentState);
                    std::cout << "Waiting for piece to be placed back on board..." << std::endl;
                    delay(50);
                }
                // move finsihed and can be stored/sent
                digitalWrite(pinLedBoardsTurn, LOW); // Turn off "board's turn" indicator
                memcpy(lastMoveState, currentState, sizeof(lastMoveState));
                sendMessage((playcodeString + "/move/board").c_str(), move.c_str());
                currentGameState = WAITING_FOR_OPPONENT_MOVE;
                
                currentPiecesCount--;
                break;
            }
            std::cout << "One of the if statements should be true here, error when this is printed" << std::endl;
        }
    }


    if (detectChange(lastMoveState, currentState) == std::make_pair(-1, -1)){

    }
}


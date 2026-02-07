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

/// @brief Moves a piece from one position to another using the stepper motors and electromagnet
/// @param fromX Location of the piece to be moved on the X axis (A-H)
/// @param fromY Location of the piece to be moved on the Y axis (1-8)
/// @param toX Location to move the piece to on the X axis (A-H)
/// @param toY Location to move the piece to on the Y axis (1-8)
/// @note Expects the coordinates in 0-7 range
/// @note Expect the 'to' location to be empty 
/// @note Expects the starting position of the magnet to be at home (A8 -> 0,0)
void movePiece(int fromX, int fromY, int toX, int toY, int magnetPin) {
    Serial.printf("Move from (%d, %d) to (%d, %d)\n", fromX, fromY, toX, toY);
    // return;
    int squaresToMoveX = abs(fromX - toX);
    int squaresToMoveY = abs(fromY - toY);

    if (fromX != 0) {
        // magent to the right
        stepperMotor2.moveRight(fromX * stepperMotor2.stepsPerSquare);        
    }

    if (fromY != 0) {
        // magnet down
        stepperMotor1.moveRight(fromY * stepperMotor1.stepsPerSquare);        
    }

    digitalWrite(magnetPin, HIGH); // Activate magnet to pick up piece





    // Move the piece in the middle of two pieces so it can be safely mved 
    // without colliding with other pieces
    if (toX != 7 || fromX != 7) {
        stepperMotor2.moveRight(stepperMotor2.stepsPerSquare / 2);
    }
    else {
        stepperMotor2.moveLeft(stepperMotor2.stepsPerSquare / 2);
    }

    if (toX != -1){
        // move y-axis
        if (toY > fromY){  stepperMotor1.moveRight(squaresToMoveY * stepperMotor1.stepsPerSquare); }
        if (toY < fromY){  stepperMotor1.moveLeft(squaresToMoveY * stepperMotor1.stepsPerSquare); }

        if (toX > fromX){  stepperMotor2.moveRight(squaresToMoveX * stepperMotor2.stepsPerSquare); }
        if (toX < fromX){  stepperMotor2.moveLeft(squaresToMoveX * stepperMotor2.stepsPerSquare); }
    }
    else{
        // Alleen bij capture
        std::cout << "Moving piece off the board due to capture..." << std::endl;
        stepperMotor1.moveRight(8 - fromY * stepperMotor1.stepsPerSquare); // move to bottom of board
        digitalWrite(magnetPin, LOW); // Deactivate magnet to release piece
        stepperMotor1.moveLeft(8 - fromY * stepperMotor1.stepsPerSquare);
        delay(1000); // wait for the piece to be moved off the board
    }
    // Move the piece in the middle of two pieces so it can be safely mved 
    // without colliding with other pieces
    if (toX != 7 || fromX != 7) {
        stepperMotor2.moveLeft(stepperMotor2.stepsPerSquare / 2);
    }
    else {
        stepperMotor2.moveRight(stepperMotor2.stepsPerSquare / 2);
    }

    
    if (toX != -1){
        digitalWrite(magnetPin, LOW); // Deactivate magnet to release piece
        stepperMotor1.moveLeft(toY * stepperMotor1.stepsPerSquare);
        stepperMotor2.moveLeft(toX * stepperMotor2.stepsPerSquare);
    }

    std::cout << "Move completed." << std::endl;
}








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
                delay(200); // delay to prevent detecting the piece places doen instantly
            }
            break;
        }

        case FIRST_MOVE_DETECTED: {
            // if the amount of pieces is the same as before after a piece was picked up, it means 
            // auto change = detectChange(firstPiecePickedUp, currentState);

            // if (change == std::make_pair(-1, -1)){
            //     std::cout << "not places down yet" << std::endl;
            //     return;
            // }
            // the piece was placed without capturing
            if (countPieces(currentState) == currentPiecesCount){
                digitalWrite(pinLedBoardsTurn, LOW); // Turn off "board's turn" indicator
                
                std::string move = createMoveStr(lastMoveState, currentState);
                std::cout << "Detected move: " + move << " detected " << countPieces(currentState)<< "/" << currentPiecesCount << " pieces" << std::endl;
                sendMessage((playcodeString + "/move/board").c_str(), move.c_str());

                memcpy(lastMoveState, currentState, sizeof(lastMoveState));
                currentGameState = WAITING_FOR_OPPONENT_MOVE;
                currentMoveState = NO_MOVE;
                break;
            }

            // A second piece is picked up which indicated a capture
            // The correct location will be set manually here so the board state is up 
            // to date before sending the move to the dashboard
            if (countPieces(currentState) == currentPiecesCount - 2){
                std::string move = createMoveStr(lastMoveState, firstPiecePickedUp, currentState);
                std::cout << "Detected move with capture: " + move << " detected " << countPieces(currentState) << " pieces" << std::endl;

                // This should not take long
                // this is blocking so the current state does not have the be stored in another variable
                // if this loop takes too long another case should be added to keep the system responsive
                while (countPieces(currentState) != currentPiecesCount - 1){
                    readBoardState(MULTIPLEXERS_COUNT, MULTIPLEXER_CHANNELS_COUNT, currentState);
                    std::cout << "Waiting for piece to be placed back on board..." << std::endl;
                    delay(50);
                }
                std::cout << "Piece placed back on board after capture." << std::endl;
                lastMoveWasCapture = true;
                // move finsihed and can be stored/sent
                digitalWrite(pinLedBoardsTurn, LOW); // Turn off "board's turn" indicator
                memcpy(lastMoveState, currentState, sizeof(lastMoveState));
                sendMessage((playcodeString + "/move/board").c_str(), move.c_str());
                currentGameState = WAITING_FOR_OPPONENT_MOVE;
                currentMoveState = NO_MOVE;
                Serial.println("piece captured 2");
                readBoardState(MULTIPLEXERS_COUNT, MULTIPLEXER_CHANNELS_COUNT, lastMoveState);
                currentPiecesCount = countPieces(lastMoveState);
                // currentPiecesCount--;
                break;
            }
            std::cout << "Waiting for piece to be placed back on board... or a second piece to be picked up (for capture)" << std::endl;
        }
    }


    if (detectChange(lastMoveState, currentState) == std::make_pair(-1, -1)){

    }
}


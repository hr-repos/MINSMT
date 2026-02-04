#include "logic.h"
#include <iostream>
#include <cstring>
// #include "Arduino.h"

/**
 * @brief Checks if the board is in the starting position
 * @param board A 2D array representing the board state
 * @return true if the board is in the starting position, false otherwise
 */
template <size_t N>
bool isStartPosition(const bool (&board)[N][N]){
    if (N != 8) {
        // std::cout << "Invalid board size. Only 8x8 boards are supported." << std::endl;
    }
    for (int row = 0; row < N; row++)
    {
        for (int column = 0; column < N; column++) 
        {
            if (row == 0 || row == 1 || row == N - 2 || row == N - 1) 
            {
                // If the first two or last two rows are missing a piece, return false
                if(board[row][column] == false) 
                {
                    std::cout << "Missing piece at row " << row << ", column " << column << std::endl;
                    // if (row != 6 && column != 0){
                        // std::cout << "Debug info: position (6,0) is false" << std::endl;
                        return false;
                    // }
                } 
            } 
            else
            {
                // If any of the middle rows have a piece, return false
                if(board[row][column] == true) 
                {
                    std::cout << "Unexpected piece at row " << row << ", column " << column << std::endl;
                    return false;
                } 
            }
        }
    }

    // If all checks passed, the board is in the start position
    return true;
}

/// @brief Generates a random play code which will be used as mqtt topic
/// @return The generated play code
int generatePlayCode(){
    return (rand() % (9999 - 1000 + 1)) + 1000;
}

/// @brief This function creates a move string based on the board state before and after a move (eg. e2e4)
/// @param before The board state before the move
/// @param after The board state after the move
/// @return Move string in algebraic notation (e.g., "e2e4") or empty string if no move detected
template <size_t N>
std::string createMoveStr(const bool (&before)[N][N], const bool (&after)[N][N]){
    int fromY = -1, toY = -1;
    char fromX = 0, toX = 0;
    bool fromFound = false, toFound = false;
    
    for (int row = 0; row < N; row++)
    {
        for (int column = 0; column < N; column++) 
        {
            if (before[row][column] != after[row][column]){
                if (before[row][column] == true && after[row][column] == false){
                    // piece moved from this position
                    fromX = 'a' + column;
                    fromY = N - row;
                    fromFound = true;
                } else if (before[row][column] == false && after[row][column] == true) {
                    // piece moved to this position
                    toX = 'a' + column;
                    toY = N - row;
                    toFound = true;
                }
            }
        }
    }
    
    // Only return move string if both from and to positions were found
    std::string s;
    s.push_back(fromX);
    s += std::to_string(fromY);
    s.push_back(toX);
    s += std::to_string(toY);
    if (fromFound && toFound) {
        return s;
    }
    
    // Return empty string if no valid move detected
    std::cout << "Error creating move string: invalid positions detected. Move: "  << std::endl;
    std::cout << "\tFrom: " << fromX << ";" << fromY << ", To: " << toX <<";"<< toY << std::endl;
    return "-1";
}

/// @brief This function creates a move string based on the board state before, inbetween and 
/// after a move with capture (eg. e4d5)
/// @tparam N is the width and height of the board
/// @param before The state before a move has been made
/// @param firstPiecePickedUp The state when the first piece was picked up
/// @param secondPiecePickedUp The state when the second piece was picked up
/// @return 
template <size_t N>
std::string createMoveStr(const bool (&before)[N][N], const bool (&firstPiecePickedUp)[N][N],const bool (&secondPiecePickedUp)[N][N]){
    std::pair<int, int> fromPos = detectChange(before, firstPiecePickedUp);
    std::pair<int, int> toPos = detectChange(firstPiecePickedUp, secondPiecePickedUp);

    if (fromPos != std::make_pair(-1, -1) && toPos != std::make_pair(-1, -1)) {
        char fromX = 'a' + fromPos.second;
        int fromY = N - fromPos.first;
        char toX = 'a' + toPos.second;
        int toY = N - toPos.first;

        std::string s;
        s.push_back(fromX);
        s += std::to_string(fromY);
        s.push_back(toX);
        s += std::to_string(toY);
        return s;
    }
    
    std::cout << "Error creating move string with capture: invalid positions detected." << std::endl;
    // Return empty string if no valid move detected
    return "-1";
}




/// @brief Checks if a move has been made between two board states
/// @tparam N is the width and height of the board
/// @param before is the board state before the move
/// @param after  is the board state after the move
/// @return true if a move has been made, false otherwise
template <size_t N>
bool hasMoved(const bool (&before)[N][N], const bool (&after)[N][N]){
    int counter = 0;
    bool changed = false;
    for (int row = 0; row < N; row++)
    {
        for (int column = 0; column < N; column++) 
        {
            if (before[row][column] != after[row][column]){
                changed = true;
            }
            if (after[row][column] == true){
                counter++;
            }
        }
    }
    std::cout << "Checking for move..., Counter: " << counter << " changed: " << changed << std::endl;
    // only return true if exactly one piece has moved (32 pieces should be on the board)
    // the counter makes sure that if the piece is picked up but not placed back, it does not count as a move
    return changed && counter == 32;
}

template <size_t N>
void movetest(const bool (&before)[N][N], const bool (&after)[N][N]){

}


/// @brief This function detects a single change between two board states and returns the coordinates of the changed cell 
/// @tparam N is the width and height of the board
/// @param first The first board change
/// @param second the next board state
/// @return std::pair<row, column> of the changed cell, or (-1, -1) if no change detected
template <size_t N>
std::pair<int, int> detectChange(const bool (&first)[N][N], const bool (&second)[N][N]){
    for (int row = 0; row < N; row++)
    {
        for (int column = 0; column < N; column++) 
        {
            if (first[row][column] != second[row][column]){
                return std::make_pair(row, column);
            }
        }
    }
    return std::make_pair(-1, -1); // No move detected
}

template <size_t N>
int countPieces(const bool (&first)[N][N]){
    int counter = 0;
    for (int row = 0; row < N; row++)
    {
        for (int column = 0; column < N; column++) 
        {
            if (first[row][column] == true){
                counter++;
            }
        }
    }
    return counter;
}



// Explicit template instantiations for common board sizes
template bool isStartPosition<8>(const bool (&board)[8][8]);
template std::string createMoveStr<8>(const bool (&before)[8][8], const bool (&after)[8][8]);
template bool hasMoved<8>(const bool (&before)[8][8], const bool (&after)[8][8]);
template std::pair<int, int> detectChange<8>(const bool (&before)[8][8], const bool (&after)[8][8]);
template int countPieces<8>(const bool (&first)[8][8]);
template std::string createMoveStr<8>(const bool (&before)[8][8], const bool (&firstPiecePickedUp)[8][8],const bool (&secondPiecePickedUp)[8][8]);

void moveStrToCoords(char* moveStr, int& fromX, int& fromY, int& toX, int& toY){
    // Example moveStr: "e2e4"
    fromX = moveStr[0] - 'a'; // Convert 'a'-'h' to 0-7
    fromY = moveStr[1] - '1'; // Convert '1'-'8' to 0-7
    toX = moveStr[2] - 'a';   // Convert 'a'-'h' to 0-7
    toY = moveStr[3] - '1';   // Convert '1'-'8' to 0-7
}

bool validateMoveStr(char* movestr){
    // Basic validation for move string format (e.g., "e2e4")
    if (strlen(movestr) != 4) {
        return false;
    }
    if (movestr[0] < 'a' || movestr[0] > 'h' || movestr[2] < 'a' || movestr[2] > 'h') {
        return false;
    }
    if (movestr[1] < '1' || movestr[1] > '8' || movestr[3] < '1' || movestr[3] > '8') {
        return false;
    }
    return true;
}
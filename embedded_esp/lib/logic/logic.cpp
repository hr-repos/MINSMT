#include "logic.h"
#include <iostream>

/**
 * @brief Checks if the board is in the starting position
 * @param board A 2D array representing the board state
 * @return true if the board is in the starting position, false otherwise
 */
template <size_t N>
bool isStartPosition(const bool (&board)[N][N]){
    if (N != 8) {
        std::cout << "Invalid board size. Only 8x8 boards are supported." << std::endl;
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
    if (fromFound && toFound) {
        std::string s;
        s.push_back(fromX);
        s += std::to_string(fromY);
        s.push_back(toX);
        s += std::to_string(toY);
        return s;
    }
    
    // Return empty string if no valid move detected
    return "";
}

// Explicit template instantiations for common board sizes
template bool isStartPosition<8>(const bool (&board)[8][8]);
template std::string createMoveStr<8>(const bool (&before)[8][8], const bool (&after)[8][8]);

void moveStrToCoords(char* moveStr, int& fromX, int& fromY, int& toX, int& toY){
    // Example moveStr: "e2e4"
    fromX = moveStr[0] - 'a'; // Convert 'a'-'h' to 0-7
    fromY = moveStr[1] - '1'; // Convert '1'-'8' to 0-7
    toX = moveStr[2] - 'a';   // Convert 'a'-'h' to 0-7
    toY = moveStr[3] - '1';   // Convert '1'-'8' to 0-7
}
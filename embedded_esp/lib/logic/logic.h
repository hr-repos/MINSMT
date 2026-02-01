#pragma once
#include <stdlib.h>
#include <string>
// #include "config.h"

// The logic module contains all the logic functions which DO NOT depend on Arduino.h
// This makes it possible to unit test these functions.

template <size_t N>
bool isStartPosition(const bool (&board)[N][N]);
int generatePlayCode();

template <size_t N>
std::string createMoveStr(const bool (&before)[N][N], const bool (&after)[N][N]);

template <size_t N>
bool hasMoved(const bool (&before)[N][N], const bool (&after)[N][N]);

void moveStrToCoords(char* moveStr, int& fromX, int& fromY, int& toX, int& toY);   
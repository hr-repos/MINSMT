#pragma once

#include "config.h"

// Function declarations
bool isStartPosition(bool board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]);
void callback(char* topic, byte* payload, unsigned int length);
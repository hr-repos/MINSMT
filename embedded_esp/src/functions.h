#pragma once

#include "config.h"

// mqtt.cpp
void callback(char* topic, byte* payload, unsigned int length);
void reconnectMqtt();

// boardfn.cpp
void readBoardState(bool board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]);
bool isStartPosition(bool board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]);
int generatePlayCode();
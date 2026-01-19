#pragma once

#include "config.h"

// mqtt.cpp
void callback(char* topic, byte* payload, unsigned int length);
void reconnectMqtt();
void sendMove();
void sendMessage(const char* topic, const char* message);

// boardfn.cpp
void readBoardState(bool board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]);
bool isStartPosition(bool board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]);
int generatePlayCode();
void moveLoop();
String createMoveStr(bool before [BOARDWIDTHHIGHT][BOARDWIDTHHIGHT], bool after [BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]);
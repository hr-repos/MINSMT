#pragma once
#include <Arduino.h>
#include "config.h"

// mqtt.cpp
void callback(char* topic, byte* payload, unsigned int length);
void reconnectMqtt();
void sendMove();
void sendMessage(const char* topic, const char* message);

// boardfn.cpp
void readBoardState(int muxCount, int channelCount, bool board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]);
void moveLoop();
void handleBoardMove();
void movePiece(int fromX, int fromY, int toX, int toY, int magnetPin);

#pragma once
#include <Arduino.h>
#include "config.h"

// mqtt.cpp
void callback(char* topic, byte* payload, unsigned int length);
void reconnectMqtt();
void sendMove();
void sendMessage(const char* topic, const char* message);

// boardfn.cpp
void readBoardState(bool board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]);
void moveLoop();

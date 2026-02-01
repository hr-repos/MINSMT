#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include "multiplexer.h"
#include "lcd.h"
#include "timer.h"
#include "steppermotor.h"

#define BOARDWIDTHHIGHT 8
#define MULTIPLEXERS_COUNT 4
#define MULTIPLEXER_CHANNELS_COUNT 16

#define MQTT_USER ""
#define MQTT_PASS ""
#define WIFI_SSID "iotroam"
#define WIFI_PASSWORD "Td|6s3@L&Pa<^!^1"

#define MQTT_SERVER "145.24.237.4"
#define MQTT_PORT 8883

enum gameState {
    WAITING_FOR_PLAYERS,
    WAITING_FOR_OPPONENT_MOVE,
    WAITING_FOR_BOARD_MOVE,
    CHECKMATE
};

extern bool lastState[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT];
extern Multiplexer* multiPlexers[MULTIPLEXERS_COUNT];  
extern StepperMotor stepperMotor1;
extern StepperMotor stepperMotor2;
extern WiFiClient espClient;                          
extern PubSubClient client;   
extern int playCode; 
extern String playcodeString;  
extern LcdModule lcd;   
extern gameState currentGameState;
extern Timer timerOpponentCheck;
extern uint8_t pinLedInactiveOpponentIndicator;
extern uint8_t pinLedBoardsTurn;
extern uint8_t pinMagneet;
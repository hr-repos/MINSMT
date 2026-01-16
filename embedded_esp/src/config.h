#pragma once

#include <WiFi.h>
#include <PubSubClient.h>
#include "multiplexer.h"

#define BOARDWIDTHHIGHT 8
#define MULTIPLEXERS_COUNT 4
#define MULTIPLEXER_CHANNELS_COUNT 16

#define MQTT_USER ""
#define MQTT_PASS ""
#define WIFI_SSID "iotroam"
#define WIFI_PASSWORD "Td|6s3@L&Pa<^!^1"

#define MQTT_SERVER "145.24.237.4"
#define MQTT_PORT 8883

extern bool boardPresence[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT];
// Contains multiplexer objects for all 4 multiplexers
extern Multiplexer* multiPlexers[MULTIPLEXERS_COUNT];  
extern WiFiClient espClient;                          
extern PubSubClient client;   
extern int playCode;     


enum gameState {
    WAITING_FOR_PLAYERS,
    IN_PROGRESS,
    FINISHED
};
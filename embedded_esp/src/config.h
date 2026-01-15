#include <WiFi.h>
#include <PubSubClient.h>

#define BOARDWIDTHHIGHT 8
#define MULTIPLEXERS_COUNT 4
#define MULTIPLEXER_CHANNELS_COUNT 16

#define MQTT_USER ""
#define MQTT_PASS ""


extern bool boardPresence[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT];
// Contains multiplexer objects for all 4 multiplexers
extern Multiplexer* multiPlexers[MULTIPLEXERS_COUNT];  // defined as extern in config.h
extern WiFiClient espClient;                           // defined as extern in config.h
extern PubSubClient client;   
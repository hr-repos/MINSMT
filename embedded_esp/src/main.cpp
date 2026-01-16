// Put external dependencies here:
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Put internal dependencies here:
#include "config.h"
#include "multiplexer.h"
#include "functions.h"
#include "lcd.h"

// Definitions multiplexers
#define MULTIPLEXERS_COUNT 4

// Function declarations:
void readBoardState(bool board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]);
void setup_wifi(const char *ssid, const char *password);

// stores if a piece is detected on the corresponding place on the board
bool boardPresence[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT] = {false};
// Contains multiplexer objects for all 4 multiplexers
Multiplexer* multiPlexers[MULTIPLEXERS_COUNT];  // defined as extern in config.h
WiFiClient espClient;                           // defined as extern in config.h
PubSubClient client(espClient);                 // defined as extern in config.h

LcdModule lcd;                                  // LCD module object

int playCode = generatePlayCode();              // defined as extern in config.h // unique play code for this game session - is used as mqtt topic
gameState currentGameState = WAITING_FOR_PLAYERS;

void setup()
{
    Serial.begin(115200);

    // Initialize multiplexers all sharing the same select pins. The signal pins should be unique
    multiPlexers[0] = new Multiplexer(19, 18, 5, 17, 27); // Mux for rows 1 and 2
    multiPlexers[1] = new Multiplexer(19, 18, 5, 17, 14); // Mux for rows 3 and 4
    multiPlexers[2] = new Multiplexer(19, 18, 5, 17, 12); // Mux for rows 5 and 6
    multiPlexers[3] = new Multiplexer(19, 18, 5, 17, 13); // Mux for rows 7 and 8
    lcd.initialize();
    
    readBoardState(boardPresence);
    lcd.setTextFirstLine("Reset the board to start playing");
    delay(2000);

    // If the board is (re)started it must be in the starting position to continue
    while (isStartPosition(boardPresence))
    {
        Serial.println("Waiting for players to set up the board...");
        delay(2000);
        readBoardState(boardPresence);
    }
    Serial.println("Board set up correctly.");

    lcd.setTextFirstLine("Share this code to start playing:");
    lcd.setTextSecondLine(String(playCode).c_str());

    setup_wifi(WIFI_SSID, WIFI_PASSWORD);
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);
}

void loop()
{
    // MQTT reconnect
    if (!client.connected()) {
        Serial.println("mqtt disconnected");
        reconnectMqtt();
    }
    client.loop();

    // if the board may move the moveLoop is called
    if (currentGameState == WAITING_FOR_BOARD_MOVE){
        moveLoop();
    }
}




void setup_wifi(const char* ssid, const char* password)
{
    Serial.print("MAC address: ");
    Serial.println(WiFi.macAddress());
    Serial.print("Connecting to ");

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }

    Serial.print("\nWiFi connected. ");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
}




// Put external dependencies here:
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Put internal dependencies here:
#include "multiplexer.h"
#include "config.h"
#include "functions.h"

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


void setup()
{
    Serial.begin(115200);

    // Initialize multiplexers all sharing the same select pins. The signal pins should be unique
    multiPlexers[0] = new Multiplexer(19, 18, 5, 17, 27); // Mux for rows 1 and 2
    multiPlexers[1] = new Multiplexer(19, 18, 5, 17, 14); // Mux for rows 3 and 4
    multiPlexers[2] = new Multiplexer(19, 18, 5, 17, 12); // Mux for rows 5 and 6
    multiPlexers[3] = new Multiplexer(19, 18, 5, 17, 13); // Mux for rows 7 and 8
    
    readBoardState(boardPresence);
    isStartPosition(boardPresence);
}

void loop()
{
    // put your main code here, to run repeatedly:
}


void reconnectMqtt() {
    // Loop until we're reconnected
    while (!client.connected()) 
    {
        Serial.print("Attempting MQTT connection...");

        // Attempt to connect
        if (client.connect("board", MQTT_USER, MQTT_PASS)) 
        {
            Serial.println("mqtt connected");

            // Once connected, publish an announcement...
            client.publish("board/status", "board connected");

            // ... and resubscribe
            client.subscribe("topic1");
        } else 
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void setup_wifi(const char* ssid, const char* password)
{
    Serial.print("Connecting to ");
    Serial.println(ssid);

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


/// @brief Reads the state of the entire board and updates the boardPresence array
/// @param board A 2D array representing the board state)
/// @test Serial.printf("y: %d\tx: %d\tch: %d\n", h, w, channel);
/// @note The board here is not a copy but a reference to the original array when compiled 
void readBoardState(bool board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT])
{
    for (int mPlexer = 0; mPlexer < MULTIPLEXERS_COUNT; mPlexer++)
    { 
        for (int channel = 0; channel < MULTIPLEXER_CHANNELS_COUNT; channel++)
        {
            int w = mPlexer * 2 + (channel / BOARDWIDTHHIGHT);
            int h = channel % BOARDWIDTHHIGHT;
            board[w][h] = false; //multiPlexers[mPlexer]->readChannel(channel);
            if (w == 3 && h == 5)
            {
                board[w][h] = true;
            }
        }
    }
}

// Put external dependencies here:
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Put internal dependencies here:
#include "multiplexer.h"
#include "secret.h"

// Definitions multiplexers
#define BOARDWIDTHHIGHT 8
#define MULTIPLEXERS_COUNT 4
// Definitions Wifi and MQTT
#define MQTT_HOST "145.24.237.4"
#define MQTT_PORT 8883

// Function declarations:
void readBoardState(bool board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]);
void setup_wifi(const char *ssid, const char *password);
void callback(char* topic, byte* payload, unsigned int length);

// stores if a piece is detected on the corresponding place on the board
bool boardPresence[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT] = {false};
// Contains multiplexer objects for all 4 multiplexers
Multiplexer* multiPlexers[MULTIPLEXERS_COUNT];
WiFiClient espClient;
PubSubClient client(espClient);


void setup()
{
    Serial.begin(115200);

    // Initialize multiplexers all sharing the same select pins. The signal pins should be unique
    multiPlexers[0] = new Multiplexer(19, 18, 5, 17, 16); // Mux for rows 1 and 2
    multiPlexers[1] = new Multiplexer(19, 18, 5, 17, 16); // Mux for rows 3 and 4
    multiPlexers[2] = new Multiplexer(19, 18, 5, 17, 16); // Mux for rows 5 and 6
    multiPlexers[3] = new Multiplexer(19, 18, 5, 17, 16); // Mux for rows 7 and 8

    // Met deze loop wacht de code tot de seriele monitor verbonden is voordat deze verder gaat, handig voor debuggen.
    // pinMode(LED_BUILTIN, OUTPUT);
    // digitalWrite(LED_BUILTIN, HIGH);
    // while (!Serial)
    // {
    //     digitalWrite(LED_BUILTIN, HIGH);
    //     delay(1000);
    //     digitalWrite(LED_BUILTIN, LOW);
    //     delay(1000);
    // }

    readBoardState(boardPresence);
    Serial.printf("Piece at 3,5: %d\n", boardPresence[3][5]);
}

void loop()
{
    // put your main code here, to run repeatedly:
}

void callback(char* topic, byte* payload, unsigned int length)
{
    char message[length + 1]; // +1 for null terminator
    for (int i = 0; i < length; i++) 
    {
        message[i] = (char)payload[i];
    }
    message[length] = '\0';
    // for debugging
    Serial.printf("Message arrived [%s] %s\n", topic, message);

    if (strcmp(topic, "topic1") == 0)
    {
        Serial.println("message received on topic1");
    }
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

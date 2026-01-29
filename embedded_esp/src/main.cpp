// Put external dependencies here:
#include <string.h>
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <string>

// Put internal dependencies here:
#include "logic.h"
#include "config.h"
#include "multiplexer.h"
#include "functions.h"
#include "lcd.h"
#include "timer.h"
#include "steppermotor.h"
// Definitions multiplexers
#define MULTIPLEXERS_COUNT 4

// Function declarations:
void readBoardState(bool board[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT]);
void setup_wifi(const char *ssid, const char *password);

// stores if a piece is detected on the corresponding place on the board
bool lastState[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT] = {false};
// Contains multiplexer objects for all 4 multiplexers
Multiplexer* multiPlexers[MULTIPLEXERS_COUNT];  // defined as extern in config.h
StepperMotor stepperMotor1(32,33);  // Stepper motor object
StepperMotor stepperMotor2(25,26);  // Stepper motor object
WiFiClient espClient;                           // defined as extern in config.h
PubSubClient client(espClient);                 // defined as extern in config.h

LcdModule lcd;                                  // LCD module object

int playCode = 6933;//generatePlayCode();              // defined as extern in config.h // unique play code for this game session - is used as mqtt topic
String playcodeString = String(playCode);
gameState currentGameState = WAITING_FOR_PLAYERS;

Timer timerKeepAlive(5000); // 5 seconds keepalive timer
Timer timerOpponentCheck(10000); // 10 seconds opponent check timer

uint8_t pinInactiveOpponentIndicator = BUILTIN_LED; // GPIO pin to indicate inactive opponent
uint8_t pinbuttonReset = 4; // GPIO pin to indicate inactive opponent
uint8_t pinbuttonShutdown = 16; // GPIO pin to indicate inactive opponent

void setup()
{
    Serial.begin(115200);
    Serial.println("Board starting...");

    // Initialize multiplexers all sharing the same select pins. The signal pins should be unique
    multiPlexers[0] = new Multiplexer(19, 18, 5, 17, 36); // Mux for rows 1 and 2
    multiPlexers[1] = new Multiplexer(19, 18, 5, 17, 39); // Mux for rows 3 and 4
    multiPlexers[2] = new Multiplexer(19, 18, 5, 17, 34); // Mux for rows 5 and 6
    multiPlexers[3] = new Multiplexer(19, 18, 5, 17, 35); // Mux for rows 7 and 8


    lcd.initialize();

    pinMode(pinInactiveOpponentIndicator, OUTPUT);
    
    readBoardState(lastState);
    lcd.setTextFirstLine("Reset the board to start playing");
    delay(2000);

    // If the board is (re)started it must be in the starting position to continue
    while (!(lastState))
    {
        Serial.println("\tWaiting for players to set up the board...");
        delay(2000);
        readBoardState(lastState);
        // break; // temperary break to allow testing without board
    }
    
    Serial.println("Board set up correctly.");

    lcd.setTextFirstLine("Share this code:");
    // String playcodeString = String(playCode);
    lcd.setTextSecondLine(playCode);

    setup_wifi(WIFI_SSID, WIFI_PASSWORD);
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);
    client.setKeepAlive(60);

}

void loop()
{
    // MQTT reconnect
    if (!client.connected()) 
    {
        Serial.println("mqtt disconnected");
        reconnectMqtt();
    }
    client.loop();

    if (currentGameState == WAITING_FOR_BOARD_MOVE)
    {
        bool currentState[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT];
        readBoardState(currentState);
        std::string move = createMoveStr(lastState, currentState);
        sendMessage((playcodeString + "/move/board").c_str(), move.c_str());
        memcpy(lastState, currentState, sizeof(lastState));
        currentGameState = WAITING_FOR_OPPONENT_MOVE;
    }

    if (timerKeepAlive.checkTimer()) 
    {
        sendMessage((playcodeString + "/alive/board").c_str(), "board alive");
        stepperMotor2.moveLeft(700);
        stepperMotor2.moveRight(700);
    }

    if (timerOpponentCheck.checkTimer()) 
    {
        Serial.println("Opponent inactive...");
        digitalWrite(pinInactiveOpponentIndicator, HIGH);
    }

    if (pinbuttonReset == HIGH) 
    {
        ESP.restart();
    }

    if (currentGameState == CHECKMATE) 
    {
        
        delay(5000);
        while(true)
        {
            if (digitalRead(pinbuttonReset) == HIGH) 
            {
                ESP.restart();
            }
        }   
    }
    // stepperMotor2.moveLeft(5000);
    // stepperMotor2.moveRight(500);
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




// Put external dependencies here:
#include <string.h>
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <string>
#include <iostream>

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
bool lastLastMoveState[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT] = {false};
bool lastMoveState[BOARDWIDTHHIGHT][BOARDWIDTHHIGHT] = {false};
// Contains multiplexer objects for all 4 multiplexers
Multiplexer* multiPlexers[MULTIPLEXERS_COUNT];  // defined as extern in config.h
StepperMotor stepperMotor1(25,26, 0, 200);  // 1-7 as 
StepperMotor stepperMotor2(32,33, 2, 200);  // a-h as (met de magneet)
WiFiClient espClient;                           // defined as extern in config.h
PubSubClient client(espClient);                 // defined as extern in config.h

LcdModule lcd;                                  // LCD module object

int playCode = 6933;//generatePlayCode();              // defined as extern in config.h // unique play code for this game session - is used as mqtt topic
String playcodeString = String(playCode);
gameState currentGameState = WAITING_FOR_PLAYERS;// WAITING_FOR_PLAYERS;
moveState currentMoveState = NO_MOVE;

Timer timerKeepAlive(5000); // 5 seconds keepalive timer
Timer timerOpponentCheck(10000); // 10 seconds opponent check timer
int currentPiecesCount = 32;
bool lastMoveWasCapture = false;

uint8_t pinMagneet = 13; // GPIO pin to indicate inactive opponent
uint8_t pinbuttonReset = 16; // GPIO pin to indicate inactive opponent
uint8_t pinbuttonShutdown = 16; // GPIO pin to indicate inactive opponent
uint8_t pinLedBoardsTurn = 27; // GPIO pin to indicate inactive opponent
uint8_t pinLedInactiveOpponentIndicator = 14; // GPIO pin to indicate inactive opponent

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

    pinMode(pinLedInactiveOpponentIndicator, OUTPUT);
    pinMode(pinLedBoardsTurn, OUTPUT);
    pinMode(pinMagneet, OUTPUT);
    
    readBoardState(MULTIPLEXERS_COUNT, MULTIPLEXER_CHANNELS_COUNT, lastMoveState);
    lcd.setTextFirstLine("Setup the board");
    lcd.setTextSecondLine("to start playing");
    delay(2000);

    // If the board is (re)started it must be in the starting position to continue
    while (!isStartPosition(lastMoveState))
    {
        Serial.println("\tWaiting for players to set up the board...");
        delay(2000);
        readBoardState(MULTIPLEXERS_COUNT, MULTIPLEXER_CHANNELS_COUNT, lastMoveState);
        // break; // temperary break to allow testing without board
    }
    
    Serial.println("Board set up correctly.");
    lcd.clearDisplay();
    lcd.setTextFirstLine("Share this code:");
    lcd.setTextSecondLine(playCode);

    setup_wifi(WIFI_SSID, WIFI_PASSWORD);
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);
    client.setKeepAlive(600);

}
bool firstMoveDetected = false;
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
        handleBoardMove();
    }

    if (timerKeepAlive.checkTimer()) 
    {
        sendMessage((playcodeString + "/alive/board").c_str(), "board alive");
        // stepperMotor1.moveLeft(300);
        // stepperMotor1.moveRight(300);
        // Serial.println("moved");
    }

    if (timerOpponentCheck.checkTimer()) 
    {
        Serial.println("Opponent inactive...");
        digitalWrite(pinLedInactiveOpponentIndicator, HIGH);
    }

    if (currentGameState == ILLEGAL_MOVE) 
    {
        if (digitalRead(pinbuttonReset) == HIGH){
            Serial.println("Reset button pressed. Restarting...");
            if (lastMoveWasCapture == true){
                currentPiecesCount +=1;
                std::cout << "Restored captured piece. Current pieces: " << currentPiecesCount << std::endl;
            }
            lcd.clearDisplay();
            lcd.setTextFirstLine("Game active!");
            readBoardState(MULTIPLEXERS_COUNT, MULTIPLEXER_CHANNELS_COUNT, lastMoveState);
            currentGameState = WAITING_FOR_BOARD_MOVE;
            digitalWrite(pinLedBoardsTurn, HIGH); // Turn on "board's turn" indicator
        }
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




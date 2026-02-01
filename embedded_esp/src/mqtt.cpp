#include "functions.h"
#include "config.h"
#include "logic.h"

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
            // client.publish("board/status", "board connected");

            // ... and resubscribe
            // client.subscribe("topic1");
            Serial.println("Subscribing to topics:");
            String topicAlive = playcodeString + "/alive/external";
            String topicMove = playcodeString + "/move/external";
            String topicCheckmate = playcodeString + "/checkmate";
            String boardColor = playcodeString + "/boardcolor";

            Serial.println(topicAlive);
            Serial.println(topicMove);
            Serial.println(topicCheckmate);
            Serial.println(boardColor);
            client.subscribe(topicAlive.c_str());
            client.subscribe(topicMove.c_str());
            client.subscribe(topicCheckmate.c_str());
            client.subscribe(boardColor.c_str());
        } else 
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(2000);
        }
    }
}


/**
 * @brief Callback function to handle incoming MQTT messages
 * @param topic The topic on which the message was received
 * @param payload The message payload
 * @param length The length of the payload
 */
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

    if (strcmp(topic, (playcodeString + "/move/external").c_str() ) == 0)
    {
        Serial.println("The opponent has made a move: " + String(message));
        int fromX, fromY, toX, toY;
        moveStrToCoords(message, fromX, fromY, toX, toY);
        Serial.printf("Move from (%d, %d) to (%d, %d)\n", fromX, fromY, toX, toY);

        // check if a piece is captured and move it first if so
        if (lastState[toX][toY]) {
            stepperMotor1.moveRight(toX * stepperMotor1.stepsPerSquare);
            stepperMotor2.moveRight(toY * stepperMotor2.stepsPerSquare);
            digitalWrite(pinMagneet, HIGH); // Activate magnet to pick up piece
            stepperMotor1.moveLeft(50); // Small lift to avoid collision
            stepperMotor2.moveRight(stepperMotor2.stepsPerSquare - toY * stepperMotor2.stepsPerSquare); // Small lift to avoid collision
            // code to move back to home here
        }


        currentGameState = WAITING_FOR_BOARD_MOVE;
        digitalWrite(pinLedBoardsTurn, HIGH); // Turn on "board's turn" indicator
    }

    if (strcmp(topic, (playcodeString + "/boardcolor").c_str() ) == 0){
        if (currentGameState == WAITING_FOR_PLAYERS) {
            // bij 0 mag het dashboard beginnen (is wit), bij 1 begint het bord (dashboard is zwart)
            if (message[0] == '0') {
                currentGameState = WAITING_FOR_OPPONENT_MOVE;
                digitalWrite(pinLedBoardsTurn, LOW);
            }
            else {
                currentGameState = WAITING_FOR_BOARD_MOVE;
                digitalWrite(pinLedBoardsTurn, HIGH);
            }
        }
    }

    if (strcmp(topic, (playcodeString + "/alive/external").c_str() ) == 0)
    {
        // Serial.println("The opponent is alive: " + String(message));
        timerOpponentCheck.resetTimer();
        digitalWrite(pinLedInactiveOpponentIndicator, LOW); // Test action
    }

    if (strcmp(topic, (playcodeString + "/checkmate").c_str() ) == 0)
    {
        Serial.println("Checkmate received from opponent.");
        lcd.clearDisplay();
        lcd.setTextFirstLine("Checkmate!");
        currentGameState = CHECKMATE;
    }
}

void sendMove() {
    // Placeholder for sending a move via MQTT
    const char* moveMessage = "e2e4"; // Example move
    Serial.printf("Sending move: %s\n", moveMessage);
    client.publish((playcodeString + "/move/board").c_str(), moveMessage);
}

void sendMessage(const char* topic, const char* message) {
    client.publish(topic, message);
}
#include "functions.h"
#include "config.h"
#include "logic.h"
#include <iostream>

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
            String topicCheckmate = playcodeString + "/gameover";
            String boardColor = playcodeString + "/boardcolor";
            String topicIllegalMove = playcodeString + "/illegalmove";

            Serial.println(topicAlive);
            Serial.println(topicMove);
            Serial.println(topicCheckmate);
            Serial.println(boardColor);
            Serial.println(topicIllegalMove);
            client.subscribe(topicAlive.c_str());
            client.subscribe(topicMove.c_str());
            client.subscribe(topicCheckmate.c_str());
            client.subscribe(boardColor.c_str());
            client.subscribe(topicIllegalMove.c_str());
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
        lastMoveWasCapture = false;
        if (currentGameState != WAITING_FOR_OPPONENT_MOVE) {
            Serial.println("Received move while not waiting for opponent move, ignoring.");
            return;
        }
        Serial.println("The opponent has made a move: " + String(message));
        if (!validateMoveStr(message)) {
            Serial.println("Invalid move string received: " + String(message) + ", it is ignored");
            return;
        }
        int fromX, fromY, toX, toY;
        moveStrToCoords(message, fromX, fromY, toX, toY);
        Serial.printf("Move from (%d, %d) to (%d, %d)\n", fromX, fromY, toX, toY);

        // check if a piece is captured and move it first if so
        if (lastMoveState[toX][toY]) {
            // coords must follow array indexing instead of chess notation so upper left = 0,0
            // movePiece(toX, 7-toY, -1, -1, pinMagneet); // move captured piece off the board
            movePiece(fromX, 7-fromY, toX, 7-toY, pinMagneet); // move piece to new location
            Serial.println("piece captured 1");
            currentPiecesCount--;
        }
        movePiece(fromX, 7-fromY, toX, 7-toY, pinMagneet); // move piece to new location
        delay(2000);
        std::cout << "Move completed on board. current pieces: " << currentPiecesCount << std::endl;
        readBoardState(MULTIPLEXERS_COUNT, MULTIPLEXER_CHANNELS_COUNT, lastMoveState);
        std::cout << "now reading: " << countPieces(lastMoveState) << " pieces on the board" << std::endl;
        currentGameState = WAITING_FOR_BOARD_MOVE;
        digitalWrite(pinLedBoardsTurn, HIGH); // Turn on "board's turn" indicator
    }

    if (strcmp(topic, (playcodeString + "/boardcolor").c_str() ) == 0){
        readBoardState(MULTIPLEXERS_COUNT, MULTIPLEXER_CHANNELS_COUNT, lastMoveState);
        if (currentGameState == WAITING_FOR_PLAYERS) {
            // bij 0 mag het dashboard beginnen (is wit), bij 1 begint het bord (dashboard is zwart)
            if (message[0] == '1') {
                currentGameState = WAITING_FOR_OPPONENT_MOVE;
                digitalWrite(pinLedBoardsTurn, LOW);
            }
            else {
                currentGameState = WAITING_FOR_BOARD_MOVE;
                digitalWrite(pinLedBoardsTurn, HIGH);
            }
        }
        lcd.clearDisplay();
        lcd.setTextFirstLine("Game active!");
    }

    if (strcmp(topic, (playcodeString + "/alive/external").c_str() ) == 0)
    {
        // Serial.println("The opponent is alive: " + String(message));
        timerOpponentCheck.resetTimer();
        digitalWrite(pinLedInactiveOpponentIndicator, LOW); // Test action
    }

    if (strcmp(topic, (playcodeString + "/gameover").c_str() ) == 0)
    {
        Serial.println("Game over received from opponent.");
        lcd.clearDisplay();
        lcd.setTextFirstLine("Game Over!");
        delay(3000);
        ESP.restart();
        // while (!isStartPosition(lastMoveState))
        // {
        //     Serial.println("\tWaiting for players to set up the board...");
        //     delay(2000);
        //     readBoardState(MULTIPLEXERS_COUNT, MULTIPLEXER_CHANNELS_COUNT, lastMoveState);
        //     // break; // temperary break to allow testing without board
        // }
        currentGameState = WAITING_FOR_PLAYERS;
    }
    if (strcmp(topic, (playcodeString + "/illegalmove").c_str() ) == 0)
    {
        Serial.println("Illegal move received from opponent.");
        lcd.setTextFirstLine("Illegal move!");
        lcd.setTextSecondLine(message);
        currentGameState = ILLEGAL_MOVE;
        // while(true){}
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


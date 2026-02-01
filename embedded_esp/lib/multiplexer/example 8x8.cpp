#include <Arduino.h>
#include "multiplexer.h"

#define MULTIPLEXERS_COUNT 4

int sensorPin = 4;
Multiplexer* multiPlexers[MULTIPLEXERS_COUNT];  // defined as extern in config.h

void setup() {
    Serial.begin(115200);

    // rijen geteld van boven naar beneden, moet dus bij het inlezen van de zetten omgedraait worden
    multiPlexers[0] = new Multiplexer(19, 18, 5, 17, 36); // Mux for rows 1 and 2
    multiPlexers[1] = new Multiplexer(19, 18, 5, 17, 39); // Mux for rows 3 and 4
    multiPlexers[2] = new Multiplexer(19, 18, 5, 17, 34); // Mux for rows 5 and 6
    multiPlexers[3] = new Multiplexer(19, 18, 5, 17, 35); // Mux for rows 7 and 8

    pinMode(sensorPin, INPUT_PULLUP);
}

void loop() {
    for (int i = 0; i< 4; i++){
        for (int j = 0; j < 16; j++){
            Serial.print(multiPlexers[i]->readChannel(j));
            Serial.print(" ");
        }
        Serial.println();
    }
    delay(1000);
}
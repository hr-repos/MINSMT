#include <Arduino.h>
#include "steppermotor.h"

StepperMotor stepperMotor1(25,26, 0, 200);  // 1-7 as 
StepperMotor stepperMotor2(32,33, 2, 200);  // a-h as (met de magneet)

void setup() {
    Serial.begin(115200);
}

void loop() {
    Serial.println("moving ");
    delay(5000);
    stepperMotor2.moveRight(200); // is rechts - naar H
    delay(2000);
    stepperMotor2.moveLeft(200); // is links - naar A
    delay(5000);
    stepperMotor1.moveRight(700); // is omlaag - naar 1
    delay(2000);
    stepperMotor1.moveLeft(700); // is omhoog - naar 7
    delay(15000); // Move right 400 steps
    Serial.println("done");
}


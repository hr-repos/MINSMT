#include "steppermotor.h"

StepperMotor::StepperMotor(int dirPin, int stepPin, int stepsPerSquare) : directionPin(dirPin), stepPin(stepPin), stepsPerSquare(stepsPerSquare) {
    pinMode(directionPin, OUTPUT);
    pinMode(stepPin, OUTPUT);
}

void StepperMotor::moveLeft(int steps) {
    digitalWrite(directionPin ,HIGH); // Set Dir high
    for(int i = 0; i < 200; i++) // Loop 200 times
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
    }
}

void StepperMotor::moveRight(int steps) {
    digitalWrite(directionPin, LOW); // Set Dir low
    for(int i = 0; i < 200; i++) // Loop 200 times
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(500);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(500);
    }
}

void StepperMotor::movetoPosition(int x, int y){
    
}

void StepperMotor::moveToHome(){

}
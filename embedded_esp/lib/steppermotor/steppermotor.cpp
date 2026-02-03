#include "steppermotor.h"
#include <iostream>

StepperMotor::StepperMotor(int dirPin, int stepPin, int sleepPin, int stepsPerSquare) : 
directionPin(dirPin), stepPin(stepPin), sleepPin(sleepPin), stepsPerSquare(stepsPerSquare) {
    pinMode(directionPin, OUTPUT);
    pinMode(stepPin, OUTPUT);
    pinMode(sleepPin, OUTPUT);
    digitalWrite(sleepPin ,LOW); // Wake up the motor driver
}

void StepperMotor::moveLeft(int steps) {
    std::cout << "Moving left " << steps << " steps." << std::endl;
    digitalWrite(directionPin ,HIGH); // Set Dir high
    digitalWrite(sleepPin ,HIGH); // Set Dir high
    for(int i = 0; i < steps; i++) // Loop 200 times
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(1000);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(1000);
    }
    digitalWrite(sleepPin ,LOW); // Set Dir high

}

void StepperMotor::moveRight(int steps) {
    std::cout << "Moving right " << steps << " steps." << std::endl;
    digitalWrite(directionPin, LOW); // Set Dir low
    digitalWrite(sleepPin ,HIGH); // Set Dir high

    for(int i = 0; i < steps; i++) // Loop 200 times
    {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(1000);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(1000);
    }
    digitalWrite(sleepPin ,LOW); // Set Dir high
}

/// @brief Moves the stepper motor a specified number of steps in either direction
/// @param steps Number of steps to move (positive for right, negative for left)
void StepperMotor::move(int steps) {
    if (steps > 0) {
        moveRight(steps);
    } else if (steps < 0) {
        moveLeft(-steps);
    }
}

void StepperMotor::movetoPosition(int x, int y){
    
}

void StepperMotor::moveToHome(){

}
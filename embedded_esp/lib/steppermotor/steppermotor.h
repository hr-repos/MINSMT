#ifndef LIB_STEPPERMOTOR_H_
#define LIB_STEPPERMOTOR_H_

// Enter dependencies here
#include <Arduino.h>

class StepperMotor {
    private:
        int directionPin;
        int stepPin;
    public:
        StepperMotor(int dirPin, int stepPin); // Constructor
        void moveLeft(int steps);
        void moveRight(int steps);
};


#endif  // LIB_STEPPERMOTOR_H_
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
        void movetoPosition(int x, int y);
        void moveLeft(int steps);
        void moveRight(int steps);
        void moveToHome();
};


#endif  // LIB_STEPPERMOTOR_H_
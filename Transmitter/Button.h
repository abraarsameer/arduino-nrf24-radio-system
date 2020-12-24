#ifndef BUTTON_H
#define BUTTON_H

#include "Arduino.h"

#define ButtonNothing 0
#define ButtonTouched 1
#define ButtonHeld    2

#define buttonDebounceDelay 50
#define buttonHoldDelay 500

typedef enum {NONE, UP, DOWN, LEFT, RIGHT, LEFT_HOLD, RIGHT_HOLD} buttonState_t;

class Button {
  private:
    unsigned long lastMillisHigh;
    byte buttonState;
    bool lastState, currentState;
    byte pin;
  public:
    Button();
    void begin(byte pin);
    byte update();
};

#endif
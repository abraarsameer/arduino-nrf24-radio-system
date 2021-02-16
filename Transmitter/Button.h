#ifndef BUTTON_H
#define BUTTON_H

#include "Arduino.h"

#define RIGHTBUTTON_PIN 8
#define DOWNBUTTON_PIN 6
#define UPBUTTON_PIN 5
#define LEFTBUTTON_PIN 7

#define ButtonNothing 0
#define ButtonTouched 1
#define ButtonHeld 2

#define buttonDebounceDelay 50
#define buttonHoldDelay 500

typedef enum {
  NONE,
  UP,
  DOWN,
  LEFT,
  RIGHT,
  LEFT_HOLD,
  RIGHT_HOLD
} buttonState_t;

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

extern Button upbtn, downbtn, leftbtn, rightbtn;

void initButtons();
buttonState_t getButtonState();

#endif
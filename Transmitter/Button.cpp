#include "Button.h"

Button upbtn, downbtn, leftbtn, rightbtn;

Button::Button() {}

void Button::begin(byte pin) {
  this->pin = pin;
  pinMode(pin, INPUT_PULLUP);
}

byte Button::update() {
  currentState = !digitalRead(pin);
  buttonState = ButtonNothing;

  if (currentState != lastState) {
    lastState = currentState;

    if (!currentState) {
      unsigned long delay = millis() - lastMillisHigh;

      if (delay > buttonDebounceDelay && delay < buttonHoldDelay) {
        buttonState = ButtonTouched;
      }

    } else {
      lastMillisHigh = millis();
    }
  } else {
    if (currentState) {
      unsigned long delay = millis() - lastMillisHigh;

      if (delay > buttonHoldDelay)
        buttonState = ButtonHeld;
    }
  }


  return buttonState;
}

void initButtons() {
  upbtn.begin(UPBUTTON_PIN);
  downbtn.begin(DOWNBUTTON_PIN);
  leftbtn.begin(LEFTBUTTON_PIN);
  rightbtn.begin(RIGHTBUTTON_PIN);
}

buttonState_t getButtonState() {
  byte up = upbtn.update();
  byte down = downbtn.update();
  byte left = leftbtn.update();
  byte right = rightbtn.update();

  if (up == ButtonTouched && !down && !left && !right) {
    return UP;
  } else if (!up && down == ButtonTouched && !left && !right) {
    return DOWN;
  } else if (!up && !down && left == ButtonTouched && !right) {
    return LEFT;
  } else if (!up && !down && !left && right == ButtonTouched) {
    return RIGHT;
  } else if (!up && !down && left == ButtonHeld && !right) {
    return LEFT_HOLD;
  } else if (!up && !down && !left && right == ButtonHeld) {
    return RIGHT_HOLD;
  } else {
    return NONE;
  }
}
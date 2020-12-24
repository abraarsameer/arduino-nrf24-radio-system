#include "Button.h"

unsigned long lastMillisUpdate;

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
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

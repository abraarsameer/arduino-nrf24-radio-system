#include "AnalogChannel.h"

int8_t analogReadAvg(byte pin) {

  unsigned int val = 0;

  for (byte i = 0; i < 50; i++) {
    val += analogRead(pin);
  }

  int8_t output = map(val / 50, 0, 1023, LOW_END_LIMIT, HIGH_END_LIMIT);

  return output;
}

AnalogChannel::AnalogChannel() {}

void AnalogChannel::begin(byte pin, float weight) {
  this->pin = pin;
  filter.begin(pin, weight, 10);
}

int8_t AnalogChannel::update() {
  byte val = filter.update() >> 2;
  val = constrain(val, lowEnd, highEnd);
  output = map(val, lowEnd, highEnd, LOW_END_LIMIT, HIGH_END_LIMIT);

  return output;
}

int8_t AnalogChannel::read() {
  return output;
}

void AnalogChannel::calibrate() {
  printfLCD(F("Move to low end"));
  while (1) {
    int8_t x = analogReadAvg(pin);
    printfLCD(x);

    if (getButtonState() != NONE) {
      lowEnd = x;
      break;
    }
  }
  printfLCD(F("Move to high end"));
  while (1) {
    int8_t x = analogReadAvg(pin);
    printfLCD(x);
    
    if (getButtonState() != NONE) {
      highEnd = x;
      break;
    }
  }
  printfLCD(F("Done"));
  delay(500);
  if (lowEnd >= highEnd) {
    byte val = lowEnd;
    lowEnd = highEnd;
    highEnd = val;
  }
}

#include "Filter.h"

Filter::Filter() {}

void Filter::begin(byte Pin, float Weight) {
  pin = Pin;
  weight = Weight;
  sampleTime = 0;
  lastMillis = millis();
  val = 0.0;
  data = 0.0;
}

void Filter::begin(byte Pin, float Weight, byte SampleTime) {
  pin = Pin;
  weight = Weight;
  sampleTime = SampleTime;
  lastMillis = millis();
  val = 0.0;
  data = 0.0;
}

void Filter::setWeight(float Weight) { weight = Weight; }

unsigned int Filter::update() {

  if (isStart) {
    val = (float)analogRead(pin);
    isStart = false;
  }

  if (millis() - lastMillis > sampleTime) {
    lastMillis = millis();
    data = (float)analogRead(pin);
    val = weight * data + (1 - weight) * val;
  }

  val = constrain(val, 0, 1023);
  return (unsigned int)val;
}

#include "AnalogChannel.h"

byte analogReadAvg(byte pin) {

  unsigned int val = 0;

  for (byte i = 0; i < 50; i++) {
    val += analogRead(pin);
  }

  val /= 50;

  return val >> 2;
}

AnalogChannel::AnalogChannel() {}

void AnalogChannel::begin(byte pin, byte startAddress, byte type) {
  this->pin = pin;
  this->startAddress = startAddress;
  this->type = type;
  filter.begin(pin, filterWeight, 10);
}

void AnalogChannel::load() {
  ChannelConfig config;
  EEPROM.get(startAddress, config);

  if (config.pin != pin) return;

  lowEnd = config.lowEnd;
  highEnd = config.highEnd;
  trim = config.trim;
  range = config.range;
  invert = config.invert;
}

void AnalogChannel::save() {
  ChannelConfig config;
  config.pin = pin;
  config.lowEnd = lowEnd;
  config.highEnd = highEnd;
  config.trim = trim;
  config.range = range;
  config.invert = invert;

  EEPROM.put(startAddress, config);
}

int AnalogChannel::update() {
  int val = filter.update() >> 2;
  val = constrain(val, lowEnd, highEnd);

  if (type == THROTTLE) {
    val = map(val, lowEnd, highEnd, 0, 2*range);
  } else {
    val = map(val, lowEnd, highEnd, 90 - range, 90 + range);
  }

  val += trim;
  val = constrain(val, 0, 180);

  if (invert) val = 180 - val;
  output = val;

  return output;
}

byte AnalogChannel::read() {
  return output;
}

void AnalogChannel::calibrate() {
  printfLCD(F("Move to low end"));
  while (1) {
    int x = analogReadAvg(pin);
    printfLCD(x);

    if (getButtonState() != NONE) {
      lowEnd = x;
      break;
    }
  }
  printfLCD(F("Move to high end"));
  while (1) {
    int x = analogReadAvg(pin);
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

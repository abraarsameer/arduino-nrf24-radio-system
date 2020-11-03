#include <EEPROM.h>
#include "Filter.h"

#define filterWeight 0.2

#define blockSize 6
#define configInvertBit 0

#define SERVO 1
#define THROTTLE 0

class AnalogChannel {
  private:
    Filter filter;
    byte output;
    byte pin;
    byte lowEnd, highEnd;
    byte startAddress;
    byte type;

  public:
    AnalogChannel();
    void begin(byte pin, byte startAddress, byte type);
    void load();
    void save();
    int update();
    byte read();
    void calibrate();
    byte range;
    int8_t trim;
    bool invert = false;
};

void printf(int x) {
  if (millis() - lastMillisUpdate > lcdUpdateInterval) {
    lastMillisUpdate = millis();
    lcd.setCursor(0, 1);
    lcd.print(F("   "));
    lcd.setCursor(0, 1);
    lcd.print(x);
  }
}

void printf(const __FlashStringHelper* c) {
  lcd.clear();
  lcd.home();
  lcd.print(c);
}

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

  if (EEPROM.read(startAddress) != pin) {


    printf(F("load: ID error"));
    return;
  }


  byte config = EEPROM.read(startAddress + 1);
  invert = bitRead(config, configInvertBit);

  lowEnd = EEPROM.read(startAddress + 2);
  highEnd = EEPROM.read(startAddress + 3);
  trim = EEPROM.read(startAddress + 4);
  range = EEPROM.read(startAddress + 5);
}

void AnalogChannel::save() {
  byte config = 0;
  bitWrite(config, configInvertBit, invert);

  EEPROM.update(startAddress, pin);
  EEPROM.update(startAddress + 1, config);
  EEPROM.update(startAddress + 2, lowEnd);
  EEPROM.update(startAddress + 3, highEnd);
  EEPROM.update(startAddress + 4, trim);
  EEPROM.update(startAddress + 5, range);
}

int AnalogChannel::update() {
  int val = filter.update() >> 2;
  val = constrain(val, lowEnd, highEnd);

  if (type == THROTTLE) {
    val = map(val, lowEnd, highEnd, 0, 90 + range);
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
  printf(F("Move to low end"));
  while (1) {
    int x = analogReadAvg(pin);
    printf(x);

    if (getButtonState() != NONE) {
      lowEnd = x;
      break;
    }
  }
  printf(F("Move to high end"));
  while (1) {
    int x = analogReadAvg(pin);
    printf(x);
    
    if (getButtonState() != NONE) {
      highEnd = x;
      break;
    }
  }
  printf(F("Done"));
  delay(500);
  if (lowEnd >= highEnd) {
    byte val = lowEnd;
    lowEnd = highEnd;
    highEnd = val;
  }
}

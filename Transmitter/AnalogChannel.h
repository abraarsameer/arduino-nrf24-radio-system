#ifndef ANALOGCHANNEL_H
#define ANALOGCHANNEL_H

#include <EEPROM.h>
#include "Filter.h"
#include "LCD.h"
#include "Button.h"

#define filterWeight 0.2

#define blockSize sizeof(ChannelConfig)

#define SERVO 1
#define THROTTLE 0

typedef struct {
  byte pin;
  byte lowEnd, highEnd;
  byte trim, range;
  bool invert : 1;
} ChannelConfig;

class AnalogChannel {
  private:
    Filter filter;
    byte output;
    byte pin;
    byte lowEnd = 0, highEnd = 255;
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
    byte range = 90;
    int8_t trim = 0;
    bool invert = false;
};

#endif
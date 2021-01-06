#ifndef ANALOGCHANNEL_H
#define ANALOGCHANNEL_H

#include <EEPROM.h>
#include "Filter.h"
#include "LCD.h"
#include "Button.h"

#define LOW_END_LIMIT -127
#define HIGH_END_LIMIT 127

class AnalogChannel {
  private:
    Filter filter;
    int8_t output;
    byte pin;

  public:
    int8_t lowEnd = LOW_END_LIMIT, highEnd = HIGH_END_LIMIT;

    AnalogChannel();
    void begin(byte pin, float weight);
    int8_t update();
    int8_t read();
    void calibrate();
};

#endif
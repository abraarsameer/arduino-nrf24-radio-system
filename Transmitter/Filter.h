#ifndef Filter_h
#define Filter_h

#include <Arduino.h>

class Filter{
  private:
    float val, data;
    float weight;
    byte pin;
    byte sampleTime;
    unsigned long lastMillis;
    bool isStart = true;
    
  public:
    Filter();
    void begin(byte Pin, float Weight);
    void begin(byte Pin, float Weight, byte SampleTime);
    unsigned int update();
    void setWeight(float Weight);
};

#endif

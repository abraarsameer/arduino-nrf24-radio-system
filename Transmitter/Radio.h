#ifndef RADIO_H
#define RADIO_H

#include "Arduino.h"
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "LCD.h"

#define CE_PIN 9
#define CS_PIN 10

extern RF24 radio;

extern const uint8_t address[];
extern bool moduleConnected, receiverConnected;

typedef struct
{
  byte channel[4];
  bool isFailsafeState : 1;
} ChannelData;

typedef struct 
{
  unsigned int pps : 12;
  bool outputEnabled : 1;
} TelemetryData;

extern ChannelData txData;
extern TelemetryData rxData;

void initRadio();
bool beginCommunications();

#endif
#ifndef CHANNELS_H
#define CHANNELS_H

#include "AnalogChannel.h"
#include "LCD.h"

#define CHANNEL_STABLIZATION_DELAY 100

extern AnalogChannel channel[4];

void initChannels();
void saveConfig();
void clearEEPROM();
void calibrateChannels();

#endif
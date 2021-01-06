#ifndef CHANNELS_H
#define CHANNELS_H

#include "AnalogChannel.h"
#include "LCD.h"
#include <OSFS.h>

#define CHANNEL_STABLIZATION_DELAY 100

struct ChannelConfig {
    int8_t lowEnd[4];
    int8_t highEnd[4];
};

extern AnalogChannel channel[4];

void initChannels();
void saveConfig();
void clearEEPROM();
void calibrateChannels();

#endif
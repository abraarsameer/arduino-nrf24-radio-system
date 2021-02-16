#ifndef CONFIG_H
#define CONFIG_H

#include "AnalogChannel.h"
#include "LCD.h"
#include "Mixing.h"
#include <OSFS.h>

#define CHANNEL_STABLIZATION_DELAY 100

struct ChannelConfig {
  int8_t lowEnd[4];
  int8_t highEnd[4];
  byte foo[4]; // Reserved space
};

struct GlobalConfig {
  byte currentModel;
  byte foo[3]; // Some reserved space for future additions
};

extern struct GlobalConfig config;

extern AnalogChannel channel[4];

void initChannels();
void loadModel();
void saveConfig();
void clearEEPROM();
void calibrateChannels();

#endif
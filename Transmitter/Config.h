#ifndef CONFIG_H
#define CONFIG_H

/*
EEPROM Layout

config
channelConfig
modelConfig0
modelConfig1
...
modelConfig9
...
*/

#define CONFIG_ADDR 0
#define CHANNEL_CONFIG_ADDR (CONFIG_ADDR + sizeof(Config))
#define MODEL_CONFIG_ADDR(n) (CHANNEL_CONFIG_ADDR + sizeof(ChannelConfig) + n*sizeof(ModelConfig))

#include "AnalogChannel.h"
#include "LCD.h"
#include "Mixing.h"
#include <OSFS.h>

#define CHANNEL_STABLIZATION_DELAY 100

struct ChannelConfig {
  int8_t lowEnd[4];
  int8_t highEnd[4];
};

struct Config {
  byte currentModel;
};

extern struct Config config;
extern byte currentModel;

extern AnalogChannel channel[4];

void initChannels();
void loadModel();
void saveConfig();
void clearEEPROM();
void calibrateChannels();

#endif
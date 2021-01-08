#ifndef MIXING_H
#define MIXING_H

#include "AnalogChannel.h"
#include "Radio.h"

struct ModelConfig
{
    int8_t trim[4];
    byte lowEndpoint[4];
    byte highEndpoint[4];

    //Dual rates and expos
    byte expo[4];
    byte secondaryRate[4];

    bool elevonMixEnabled;   
};

extern struct ModelConfig modelConfig;

void applyModelSettings();

#endif
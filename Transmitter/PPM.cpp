#include "PPM.h"

void updatePPM()
{
    for (byte i = 0; i < 4; i++)
    {
        unsigned int val = map(txData.channel[i], 0, 180, PPM_MIN, PPM_MAX);
        ppmEncoder.setChannel(i, val);
    }
}
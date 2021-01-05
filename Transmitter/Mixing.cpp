#include "Mixing.h"

bool elevonMixEnabled;

void updateMixes()
{
    if (elevonMixEnabled)
    {
        int8_t elevator = txData.channel[2] - 90;
        int8_t aileron = txData.channel[3] - 90;

        txData.channel[2] = constrain(elevator + aileron, -90, 90) + 90;
        txData.channel[3] = constrain(elevator - aileron, -90, 90) + 90;
    }
}
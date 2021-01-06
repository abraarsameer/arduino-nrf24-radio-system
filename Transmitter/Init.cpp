#include "Init.h"

void initAll()
{
    //Serial initialization
#ifdef SERIAL_DEBUG
    Serial.begin(115200);
    Serial.println(F("RC Transmitter Serial Debug"));
#endif

    memset(&txData, 0, sizeof(txData));
    memset(&rxData, 0, sizeof(rxData));

    initButtons();
    initLCD();
    initRadio();
    initChannels();
}
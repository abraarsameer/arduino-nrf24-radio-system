#include "Channels.h"

AnalogChannel channel[4];

void initChannels()
{
    //Channel initialization
    channel[0].begin(A0, 0 * blockSize, THROTTLE);
    channel[1].begin(A1, 1 * blockSize, SERVO);
    channel[2].begin(A2, 2 * blockSize, SERVO);
    channel[3].begin(A3, 3 * blockSize, SERVO);

    for (byte i = 0; i < 4; i++)
    {
        channel[i].load();
    }

    //Update channels for a while to stablize
    unsigned long lastMillis = millis();
    while (millis() - lastMillis < CHANNEL_STABLIZATION_DELAY)
    {
        for (byte i = 0; i < 4; i++)
            channel[i].update();
    }
}

void saveConfig()
{
    lcd.clear();
    lcd.home();

    for (byte i = 0; i < 4; i++)
    {
        channel[i].save();
    }

    lcd.print(F("Config saved"));
    delay(500);
}

void clearEEPROM()
{
    lcd.clear();
    lcd.home();
    lcd.print(F("Clearing EEPROM"));

    for (int i = 0; i < EEPROM.length(); i++)
    {
        EEPROM.update(i, 0);
    }

    lcd.clear();
    lcd.home();
    lcd.print(F("Done"));
    delay(500);
}

void calibrateChannels()
{
    for (byte i = 0; i < 4; i++)
    {
        lcd.clear();
        lcd.home();
        lcd.print(F("Calibrating"));
        lcd.setCursor(0, 1);
        lcd.print(F("Ch "));
        lcd.print(i + 1);
        delay(1000);
        channel[i].calibrate();
    }
}
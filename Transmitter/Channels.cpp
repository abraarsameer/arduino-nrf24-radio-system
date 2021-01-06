#include "Channels.h"

uint16_t OSFS::startOfEEPROM = 1;
uint16_t OSFS::endOfEEPROM = 1024;

AnalogChannel channel[4];

void initChannels()
{
    //Channel initialization
    channel[0].begin(A0, 0.2);
    channel[1].begin(A1, 0.2);
    channel[2].begin(A2, 0.2);
    channel[3].begin(A3, 0.2);

    struct ChannelConfig channelConfig;
    OSFS::getFile("channel", channelConfig);

    for (byte i = 0; i < 4; i++)
    {
        channel[i].lowEnd = channelConfig.lowEnd[i];
        channel[i].highEnd = channelConfig.highEnd[i];
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

    struct ChannelConfig channelConfig;

    for (byte i = 0; i < 4; i++)
    {
        channelConfig.lowEnd[i] = channel[i].lowEnd;
        channelConfig.highEnd[i] = channel[i].highEnd;
    }

    OSFS::newFile("channel", channelConfig, true);

    lcd.print(F("Config saved"));
    delay(500);
}

void clearEEPROM()
{
    OSFS::format();
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

void OSFS::readNBytes(uint16_t address, unsigned int num, byte* output) {
	for (uint16_t i = address; i < address + num; i++) {
		*output = EEPROM.read(i);
		output++;
	}
}

void OSFS::writeNBytes(uint16_t address, unsigned int num, const byte* input) {
	for (uint16_t i = address; i < address + num; i++) {
		EEPROM.update(i, *input);
		input++;
	}
}
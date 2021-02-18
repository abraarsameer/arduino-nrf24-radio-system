#include "Config.h"

uint16_t OSFS::startOfEEPROM = 0;
uint16_t OSFS::endOfEEPROM = 1023;

AnalogChannel channel[4];
byte currentModel;

void initConfig() {

  // Config initialization
  struct Config config;
  EEPROM.get(CONFIG_ADDR, config);
  loadModel(config.currentModel);

  // Channel initialization
  channel[0].begin(A0, 0.2);
  channel[1].begin(A1, 0.2);
  channel[2].begin(A2, 0.2);
  channel[3].begin(A3, 0.2);

  struct ChannelConfig channelConfig;
  EEPROM.get(CHANNEL_CONFIG_ADDR, channelConfig);

  for (byte i = 0; i < 4; i++) {
    channel[i].lowEnd = channelConfig.lowEnd[i];
    channel[i].highEnd = channelConfig.highEnd[i];
  }

  // Update channels for a while to stablize
  unsigned long lastMillis = millis();
  while (millis() - lastMillis < CHANNEL_STABLIZATION_DELAY) {
    for (byte i = 0; i < 4; i++)
      channel[i].update();
  }
}

void loadModel(byte i) {
  currentModel = i;
  EEPROM.get(MODEL_CONFIG_ADDR(i), modelConfig)

  if (!modelConfig.isSaved)) {
    // Default configuration 
    memset(&modelConfig.trim, 0, 4);
    memset(&modelConfig.lowEndpoint, LOW_END_LIMIT, 4);
    memset(&modelConfig.highEndpoint, HIGH_END_LIMIT, 4);
    memset(&modelConfig.expo, 0, 4);
    memset(&modelConfig.secondaryRate, 100, 4);

    modelConfig.elevonMixEnabled = false;
  }
}

void saveConfig() {
  lcd.clear();
  lcd.home();

  // Save Channel Configuration
  struct ChannelConfig channelConfig;
  memset(channelConfig.foo, 0, 4);

  for (byte i = 0; i < 4; i++) {
    channelConfig.lowEnd[i] = channel[i].lowEnd;
    channelConfig.highEnd[i] = channel[i].highEnd;
  }

  OSFS::newFile("channel", channelConfig, true);

  // Save Global Configuration
  memset(config.foo, 0, 3);
  OSFS::newFile("config", config, true);

  // Save Model Configuration
  char name[7] = "model";
  char number[2];
  itoa(config.currentModel, number, 10);
  OSFS::newFile(name, modelConfig, true);

  lcd.print(F("Config saved"));
  delay(500);
}

void clearEEPROM() {
  lcd.clear();
  lcd.home();
  OSFS::format();
  lcd.print(F("Done"));
  delay(500);
}

void calibrateChannels() {
  for (byte i = 0; i < 4; i++) {
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

void OSFS::readNBytes(uint16_t address, unsigned int num, byte *output) {
  for (uint16_t i = address; i < address + num; i++) {
    *output = EEPROM.read(i);
    output++;
  }
}

void OSFS::writeNBytes(uint16_t address, unsigned int num, const byte *input) {
  for (uint16_t i = address; i < address + num; i++) {
    EEPROM.update(i, *input);
    input++;
  }
}
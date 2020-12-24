#ifndef GLOBALS_H
#define GLOBALS_H

#include "Arduino.h"
#include "Button.h"
#include <LiquidCrystal_I2C.h>

#define lcdUpdateInterval 125

extern LiquidCrystal_I2C lcd;
extern Button upbtn, downbtn, leftbtn, rightbtn;
extern unsigned long lastMillisUpdate;

byte getButtonState();

#endif
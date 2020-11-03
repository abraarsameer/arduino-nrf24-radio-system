#include "Button.h"

#define lcdUpdateInterval 125

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
Button upbtn, downbtn, leftbtn, rightbtn;

unsigned long lastMillisUpdate;

byte getButtonState();

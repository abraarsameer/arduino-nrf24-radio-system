#ifndef LCD_H
#define LCD_H

#include "Arduino.h"
#include <LiquidCrystal_I2C.h>

#define lcdUpdateInterval 125

#define rightArrow 1
#define leftArrow 2
#define slider 3
#define antenna 4
#define txLowBatt 5
#define rxLowBatt 6

extern unsigned long lastMillisUpdate;
extern LiquidCrystal_I2C lcd;

void initLCD();

void printFloat(float val);
void printInt(int val, size_t len);
void printSlider(bool val, byte row, byte col);

void printfLCD(int x);
void printfLCD(const __FlashStringHelper* c);
void printfLCD(const __FlashStringHelper* c1, const __FlashStringHelper* c2);

#endif
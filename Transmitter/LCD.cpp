#include "LCD.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);
unsigned long lastMillisUpdate;

void initLCD() {
  lcd.begin();

  byte rightArrowChar[] = {B10000, B11000, B11100, B11110,
                           B11110, B11100, B11000, B10000
                          };

  byte leftArrowChar[] = {B00001, B00011, B00111, B01111,
                          B01111, B00111, B00011, B00001
                         };

  byte sliderChar[] = {B01110, B10001, B10101, B10101,
                       B10101, B10101, B10001, B01110
                      };

  byte antennaChar[] = {B00000, B11111, B10001, B01010,
                        B00100, B00100, B00100, B00100
                       };

  byte txLowBattChar[] = {B01110, B10001, B11111, B10101,
                          B10101, B10101, B10001, B11111
                         };

  byte rxLowBattChar[] = {B01110, B10001, B11111, B11011,
                          B11101, B11011, B10001, B11111
                         };


  lcd.createChar(rightArrow, rightArrowChar);
  lcd.createChar(leftArrow, leftArrowChar);
  lcd.createChar(slider, sliderChar);
  lcd.createChar(antenna, antennaChar);
  lcd.createChar(txLowBatt, txLowBattChar);
  lcd.createChar(rxLowBatt, rxLowBattChar);
}

void printFloat(float val) {
  char buff[4];
  dtostrf(val, 4, 1, buff);
  lcd.print(buff);
}

void printInt(int val, size_t len) {
  char buff[len+1];
  char spaces[4] = "   ";
  
  itoa(val, buff, 10);
  strncat(buff, spaces, len - strlen(buff));
  lcd.print(buff);
}

void printSlider(bool val, byte row, byte col) {
  if (val) {
    lcd.setCursor(row, col);
    lcd.print(" ");
    lcd.setCursor(row + 1, col);
    lcd.write(slider);
  } else {
    lcd.setCursor(row, col);
    lcd.write(slider);
    lcd.setCursor(row + 1, col);
    lcd.print(" ");
  }
}

void printfLCD(int x) {
  if (millis() - lastMillisUpdate > lcdUpdateInterval) {
    lastMillisUpdate = millis();
    lcd.setCursor(0, 1);
    lcd.print(F("   "));
    lcd.setCursor(0, 1);
    lcd.print(x);
  }
}

void printfLCD(const __FlashStringHelper* c) {
  lcd.clear();
  lcd.home();
  lcd.print(c);
}

void printfLCD(const __FlashStringHelper* c1, const __FlashStringHelper* c2) {
  lcd.clear();
  lcd.home();
  lcd.print(c1);
  lcd.setCursor(0, 1);
  lcd.print(c2);
}
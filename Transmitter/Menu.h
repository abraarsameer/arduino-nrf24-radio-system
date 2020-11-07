#define UPBUTTON_PIN      8
#define DOWNBUTTON_PIN    6
#define LEFTBUTTON_PIN    5
#define RIGHTBUTTON_PIN   7

#define rightArrow 1
#define leftArrow 2
#define slider 3
#define antenna 4
#define txLowBatt 5
#define rxLowBatt 6

#define txLowBattThreshold 7.0
#define rxLowBattThreshold 10.5

#define trimLowerLimit -30
#define trimUpperLimit 30

#define f(x) (reinterpret_cast<const __FlashStringHelper*>(&x[0]))
#define len(x) (sizeof(x)/sizeof(x[0]))
typedef const __FlashStringHelper* fstring;
typedef const char PROGMEM pchar;
//Menus
pchar Display[] = "Display";
pchar Trim[] = "Trim";
pchar Range[] = "Range";
pchar Invert[] = "Invert";
pchar Save[] = "Configuration";

fstring mainMenu[] = {f(Display), f(Trim), f(Range), f(Invert), f(Save)};
fstring backgroundMenu[] = {f(Display)}; //Dummy, only serves to provide an address for identification

pchar Ch1[] = "Ch 1";
pchar Ch2[] = "Ch 2";
pchar Ch3[] = "Ch 3";
pchar Ch4[] = "Ch 4";

fstring displayMenu[] = {f(Ch1), f(Ch2), f(Ch3), f(Ch4)};
fstring trimMenu[] = {f(Ch1), f(Ch2), f(Ch3), f(Ch4)};
fstring rangeMenu[] = {f(Ch1), f(Ch2), f(Ch3), f(Ch4)};
fstring invertMenu[] = {f(Ch1), f(Ch2), f(Ch3), f(Ch4)};

pchar saveMsg[] = "Save";
pchar calibrateMsg[] = "Calibrate";
pchar clearEEPROMMsg[] = "Clear EEPROM";
fstring saveMenu[] = {f(saveMsg), f(calibrateMsg), f(clearEEPROMMsg)};

fstring* subMenus[] = {displayMenu, trimMenu, rangeMenu, invertMenu, saveMenu};
byte subMenuSizes[] = {4, 4, 4, 4, 3};

/*--------------------------------*/

//Variables
fstring* currentMenu = backgroundMenu;
byte currentPos;
byte currentMenuSize;
byte lastScreen = 255;
bool optionSelected;
bool backgroundVisible;

AnalogChannel channel[4];

byte packetSucccessRate;
float transmitterVoltage;
float receiverVoltage;
//

//Functions

void saveConfig() {
  lcd.clear();
  lcd.home();

  for (byte i = 0; i < 4; i++) {
    channel[i].save();
  }

  lcd.print(F("Config saved"));
  delay(500);
}

void clearEEPROM() {
  lcd.clear();
  lcd.home();
  lcd.print(F("Clearing EEPROM"));

  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.update(i, 0);
  }

  lcd.clear();
  lcd.home();
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

void initMenu() {
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

  upbtn.begin(UPBUTTON_PIN);
  downbtn.begin(DOWNBUTTON_PIN);
  leftbtn.begin(LEFTBUTTON_PIN);
  rightbtn.begin(RIGHTBUTTON_PIN);
}



void switchMenu(fstring* menu, byte size) {
  currentMenu = menu;
  currentMenuSize = size;
  lastScreen = 255;
  optionSelected = false;

  currentPos = 0;
}

void stepBackward() {
  if (currentPos == 0) {
    currentPos = currentMenuSize - 1;
  } else {
    currentPos--;
  }
}

void stepForward() {
  if (currentPos == currentMenuSize - 1) {
    currentPos = 0;
  } else {
    currentPos++;
  }
}

int8_t increment(int8_t val, int8_t low, int8_t high) {
  val++;
  val = constrain(val, low, high);
  return val;
}

int8_t decrement(int8_t val, int8_t low, int8_t high) {
  val--;
  val = constrain(val, low, high);
  return val;
}

byte getButtonState() {
  byte up = upbtn.update();
  byte down = downbtn.update();
  byte left = leftbtn.update();
  byte right = rightbtn.update();

  if (up == ButtonTouched && !down && !left && !right) {
    return UP;
  } else if (!up && down == ButtonTouched && !left && !right) {
    return DOWN;
  } else if (!up && !down && left == ButtonTouched && !right) {
    return LEFT;
  } else if (!up && !down && !left && right == ButtonTouched) {
    return RIGHT;
  } else if (!up && !down && left == ButtonHeld && !right) {
    return LEFT_HOLD;
  } else if (!up && !down && !left && right == ButtonHeld) {
    return RIGHT_HOLD;
  } else {
    return NONE;
  }
}


void backgroundMenuCallback() {
  byte buttonState = getButtonState();

  if (buttonState != NONE) {
    switchMenu(mainMenu, len(mainMenu));
    backgroundVisible = false;
  }
}

void mainMenuCallback() {
  byte buttonstate = getButtonState();

  if (buttonstate == NONE) return;

  switch (buttonstate) {
    case UP:
      stepBackward();
      break;
    case DOWN:
      stepForward();
      break;
    case LEFT:
      switchMenu(backgroundMenu, 0);
      break;
    case RIGHT:
      switchMenu(subMenus[currentPos], subMenuSizes[currentPos]);
      break;
  }
}

void displayMenuCallback() {
  if (millis() - lastMillisUpdate > lcdUpdateInterval) {
    lastMillisUpdate = millis();
    byte currentScreen = currentPos / 2;
    int8_t val = channel[currentScreen * 2].read() - 90;
    lcd.setCursor(11, 0);
    printInt(abs(val), 3);

    if (val < 0) {
      lcd.setCursor(10, 0);
      lcd.print("-");
    } else if (val > 0) {
      lcd.setCursor(10, 0);
      lcd.print("+");
    }

    if (currentPos != currentMenuSize) {
      lcd.setCursor(11, 1);
      val = channel[currentScreen * 2 + 1].read() - 90;
      printInt(abs(val), 3);

      if (val < 0) {
        lcd.setCursor(10, 1);
        lcd.print("-");
      } else if (val > 0) {
        lcd.setCursor(10, 1);
        lcd.print("+");
      }
    }
  }

  byte buttonstate = getButtonState();
  if (buttonstate == NONE) return;

  switch (buttonstate) {
    case UP:
      stepBackward();
      break;
    case DOWN:
      stepForward();
      break;
    case LEFT:
      switchMenu(mainMenu, len(mainMenu));
      break;
    case RIGHT:
      //Nothing
      break;
  }
}

void trimMenuCallback() {
  byte buttonstate = getButtonState();

  if (millis() - lastMillisUpdate > lcdUpdateInterval) {
    lastMillisUpdate = millis();

    if (optionSelected) {
      if (buttonstate == LEFT_HOLD) {
        channel[currentPos].trim = decrement(channel[currentPos].trim, trimLowerLimit, trimUpperLimit);
      } else if (buttonstate == RIGHT_HOLD) {
        channel[currentPos].trim = increment(channel[currentPos].trim, trimLowerLimit, trimUpperLimit);
      }
    }

    byte currentScreen = currentPos / 2;
    int8_t val = channel[currentScreen * 2].trim;

    lcd.setCursor(11, 0);
    printInt(abs(val), 2);

    if (val < 0) {
      lcd.setCursor(10, 0);
      lcd.print("-");
    } else if (val > 0) {
      lcd.setCursor(10, 0);
      lcd.print("+");
    }

    if (currentPos != currentMenuSize) {
      lcd.setCursor(11, 1);
      val = channel[currentScreen * 2 + 1].trim;
      printInt(abs(val), 2);

      if (val < 0) {
        lcd.setCursor(10, 1);
        lcd.print("-");
      } else if (val > 0) {
        lcd.setCursor(10, 1);
        lcd.print("+");
      }
    }

    byte row = (currentPos + 2) % 2;
    if (optionSelected) {
      lcd.setCursor(9, row);
      lcd.write(leftArrow);
      lcd.setCursor(13, row);
      lcd.write(rightArrow);
    } else {
      lcd.setCursor(9, row);
      lcd.print(" ");
      lcd.setCursor(13, row);
      lcd.print(" ");
    }
  }

  if (buttonstate == NONE) return;

  switch (buttonstate) {
    case UP:
      if (optionSelected) {
        optionSelected = false;
      } else {
        stepBackward();
      }
      break;
    case DOWN:
      if (optionSelected) {
        optionSelected = false;
      } else {
        stepForward();
      }
      break;
    case LEFT:
      if (optionSelected) {
        channel[currentPos].trim = decrement(channel[currentPos].trim, trimLowerLimit, trimUpperLimit);
      } else {
        switchMenu(mainMenu, len(mainMenu));
      }
      break;
    case RIGHT:
      if (optionSelected) {
        channel[currentPos].trim = increment(channel[currentPos].trim, trimLowerLimit, trimUpperLimit);
      } else {
        optionSelected = true;
      }
      break;
  }
}

void rangeMenuCallback() {
  byte buttonstate = getButtonState();

  if (millis() - lastMillisUpdate > lcdUpdateInterval) {
    lastMillisUpdate = millis();

    if (optionSelected) {
      if (buttonstate == LEFT_HOLD) {
        channel[currentPos].range = decrement(channel[currentPos].range, 0, 90);
      } else if (buttonstate == RIGHT_HOLD) {
        channel[currentPos].range = increment(channel[currentPos].range, 0, 90);
      }
    }

    byte currentScreen = currentPos / 2;
    int8_t val = channel[currentScreen * 2].range;

    lcd.setCursor(11, 0);
    printInt(abs(val), 2);

    if (currentPos != currentMenuSize) {
      lcd.setCursor(11, 1);
      val = channel[currentScreen * 2 + 1].range;
      printInt(abs(val), 2);
    }

    byte row = (currentPos + 2) % 2;
    if (optionSelected) {
      lcd.setCursor(10, row);
      lcd.write(leftArrow);
      lcd.setCursor(13, row);
      lcd.write(rightArrow);
    } else {
      lcd.setCursor(10, row);
      lcd.print(" ");
      lcd.setCursor(13, row);
      lcd.print(" ");
    }
  }

  if (buttonstate == NONE) return;

  switch (buttonstate) {
    case UP:
      if (optionSelected) {
        optionSelected = false;
      } else {
        stepBackward();
      }
      break;
    case DOWN:
      if (optionSelected) {
        optionSelected = false;
      } else {
        stepForward();
      }
      break;
    case LEFT:
      if (optionSelected) {
        channel[currentPos].range = decrement(channel[currentPos].range, 0, 90);
      } else {
        switchMenu(mainMenu, len(mainMenu));
      }
      break;
    case RIGHT:
      if (optionSelected) {
        channel[currentPos].range = increment(channel[currentPos].range, 0, 90);
      } else {
        optionSelected = true;
      }
      break;
  }
}

void invertMenuCallback() {
  byte currentScreen = currentPos / 2;
  bool val = channel[currentScreen * 2].invert;
  printSlider(val, 11, 0);

  if (currentPos != currentMenuSize) {
    val = channel[currentScreen * 2 + 1].invert;
    printSlider(val, 11, 1);
  }

  byte row = (currentPos + 2) % 2;
  if (optionSelected) {
    lcd.setCursor(10, row);
    lcd.write(leftArrow);
    lcd.setCursor(13, row);
    lcd.write(rightArrow);
  } else {
    lcd.setCursor(10, row);
    lcd.print(" ");
    lcd.setCursor(13, row);
    lcd.print(" ");
  }

  byte buttonstate = getButtonState();
  if (buttonstate == NONE) return;

  switch (buttonstate) {
    case UP:
      if (optionSelected) {
        optionSelected = false;
      } else {
        stepBackward();
      }
      break;
    case DOWN:
      if (optionSelected) {
        optionSelected = false;
      } else {
        stepForward();
      }
      break;
    case LEFT:
      if (optionSelected) {
        channel[currentPos].invert = !channel[currentPos].invert;
      } else {
        switchMenu(mainMenu, len(mainMenu));
      }
      break;
    case RIGHT:
      if (optionSelected) {
        channel[currentPos].invert = !channel[currentPos].invert;
      } else {
        optionSelected = true;
      }
      break;
  }
}

void saveMenuCallback() {
  byte buttonstate = getButtonState();
  if (buttonstate == NONE) return;

  switch (buttonstate) {
    case UP:
      stepBackward();
      break;
    case DOWN:
      stepForward();
      break;
    case LEFT:
      switchMenu(mainMenu, len(mainMenu));
      break;
    case RIGHT:
      if (currentPos == 0) {
        saveConfig();
      } else if (currentPos == 1) {
        calibrateChannels();
      } else if (currentPos == 2) {
        clearEEPROM();
      }
      switchMenu(mainMenu, len(mainMenu));
      break;
  }
}

void updateEntries() {
  byte currentScreen = currentPos / 2;

  if (currentScreen != lastScreen) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(currentMenu[currentScreen * 2]);

    if (currentScreen * 2 != currentMenuSize - 1) {
      lcd.setCursor(1, 1);
      lcd.print(currentMenu[currentScreen * 2 + 1]);
    }

    lastScreen = currentScreen;
  }

  if ((currentPos + 2) % 2 == 0) {
    lcd.setCursor(0, 0);
    lcd.write(rightArrow);
    lcd.setCursor(0, 1);
    lcd.print(" ");
  } else {
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.write(rightArrow);
  }
}

void displayBackground() {
  if (!backgroundVisible) {
    backgroundVisible = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(antenna);
    lcd.setCursor(5, 0);
    lcd.print("%");
    lcd.setCursor(7, 0);
    lcd.print("VTx");
    lcd.setCursor(15, 0);
    lcd.print("V");
    lcd.setCursor(7, 1);
    lcd.print("VRx");
    lcd.setCursor(15, 1);
    lcd.print("V");
  }

  if (millis() - lastMillisUpdate > 1000) {
    lastMillisUpdate = millis();
    lcd.setCursor(2, 0);
    printInt(packetSucccessRate, 3);
    lcd.setCursor(11, 0);
    printFloat(transmitterVoltage);
    lcd.setCursor(11, 1);
    printFloat(receiverVoltage);

    lcd.setCursor(0, 1);
    if (transmitterVoltage < txLowBattThreshold) {
      lcd.write(txLowBatt);
    } else {
      lcd.print(" ");
    }

    lcd.setCursor(2, 1);
    if (receiverVoltage < rxLowBattThreshold) {
      lcd.write(rxLowBatt);
    } else {
      lcd.print(" ");
    }
  }
}

void updateMenu() {
  if (currentMenu == backgroundMenu) {
    displayBackground();
  } else {
    updateEntries();
  }

  if (currentMenu == backgroundMenu) {
    backgroundMenuCallback();
  } else if (currentMenu == mainMenu) {
    mainMenuCallback();
  } else if (currentMenu == displayMenu) {
    displayMenuCallback();
  } else if (currentMenu == trimMenu) {
    trimMenuCallback();
  } else if (currentMenu == rangeMenu) {
    rangeMenuCallback();
  } else if (currentMenu == invertMenu) {
    invertMenuCallback();
  } else if (currentMenu == saveMenu) {
    saveMenuCallback();
  }
}

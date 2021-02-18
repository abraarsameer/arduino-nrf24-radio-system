#include "Menu.h"

pchar Display[] = "Display";
pchar Trim[] = "Trim";
pchar Range[] = "Range";
pchar Expo[] = "Expo";
pchar Invert[] = "Invert";
pchar Mixing[] = "Mixing";
pchar Save[] = "Configuration";
pchar RadioStats[] = "Radio Stats";

fstring mainMenu[] = {f(Display), f(Trim), f(Range), f(Expo), f(Invert), f(Mixing), f(Save), f(RadioStats)};
fstring backgroundMenu[] = {f(Display)}; //Dummy, only serves to provide an address for identification

pchar Ch1[] = "Ch 1";
pchar Ch2[] = "Ch 2";
pchar Ch3[] = "Ch 3";
pchar Ch4[] = "Ch 4";

fstring displayMenu[] = {f(Ch1), f(Ch2), f(Ch3), f(Ch4)};
fstring trimMenu[] = {f(Ch1), f(Ch2), f(Ch3), f(Ch4)};
fstring rangeMenu[] = {f(Ch1), f(Ch2), f(Ch3), f(Ch4)};
fstring expoMenu[] = {f(Ch1), f(Ch2), f(Ch3), f(Ch4)};
fstring invertMenu[] = {f(Ch1), f(Ch2), f(Ch3), f(Ch4)};

pchar elevon[] = "Elevon";

fstring mixingMenu[] =  {f(elevon)};

pchar saveMsg[] = "Save";
pchar calibrateMsg[] = "Calibrate";
pchar clearEEPROMMsg[] = "Clear EEPROM";
fstring saveMenu[] = {f(saveMsg), f(calibrateMsg), f(clearEEPROMMsg)};

fstring radioStatsMenu[] = {f(Display)};

fstring* subMenus[] = {displayMenu, trimMenu, rangeMenu, expoMenu, invertMenu, mixingMenu, saveMenu, radioStatsMenu};
byte subMenuSizes[] = {4, 4, 4, 4, 4, 1, 3, 0};

/*--------------------------------*/

//Variables
fstring* currentMenu = backgroundMenu;
byte currentPos, lastPos = 255;
byte currentMenuSize;
byte lastScreen = 255;
bool optionSelected;
bool backgroundVisible, radioStatsMenuVisible;

byte packetSucccessRate;
float transmitterVoltage, receiverVoltage;
unsigned int sentPacketsDisplay, receivedPacketsDisplay, ackedPacketsDisplay;

void switchMenu(fstring* menu, byte size) {
  currentMenu = menu;
  currentMenuSize = size;
  lastScreen = 255;
  lastPos = 255;
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


//Callback Functions

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
    int8_t val = txData.channel[currentScreen * 2] - 90;
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
      val = txData.channel[currentScreen * 2 + 1] - 90;
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


void expoMenuCallback() {
  byte buttonstate = getButtonState();

  if (millis() - lastMillisUpdate > lcdUpdateInterval) {
    lastMillisUpdate = millis();

    if (optionSelected) {
      if (buttonstate == LEFT_HOLD) {
        channel[currentPos].expo = decrement(channel[currentPos].expo, 0, 99); //Range 0-99
      } else if (buttonstate == RIGHT_HOLD) {
        channel[currentPos].expo = increment(channel[currentPos].expo, 0, 99);
      }
    }

    byte currentScreen = currentPos / 2;
    int8_t val = channel[currentScreen * 2].expo;

    lcd.setCursor(11, 0);
    printInt(abs(val), 2);

    if (currentPos != currentMenuSize) {
      lcd.setCursor(11, 1);
      val = channel[currentScreen * 2 + 1].expo;
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
        channel[currentPos].expo = decrement(channel[currentPos].expo, 0, 99);
      } else {
        switchMenu(mainMenu, len(mainMenu));
      }
      break;
    case RIGHT:
      if (optionSelected) {
        channel[currentPos].expo = increment(channel[currentPos].expo, 0, 99);
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

void mixingMenuCallback() {
  byte currentScreen = currentPos / 2;
  bool val = currentPos == 0 ? elevonMixEnabled : 0;
  printSlider(val, 11, 0);

  /*if (currentPos != currentMenuSize) {
    val = channel[currentScreen * 2 + 1].invert;
    printSlider(val, 11, 1);
  }*/

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
        if(currentPos == 0) {
          elevonMixEnabled = !elevonMixEnabled;
        }
      } else {
        switchMenu(mainMenu, len(mainMenu));
      }
      break;
    case RIGHT:
      if (optionSelected) {
        if (currentPos == 0) {
          elevonMixEnabled = !elevonMixEnabled;
        }
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

void radioStatsMenuCallback() {
  byte buttonState = getButtonState();

  if (buttonState != NONE) {
    switchMenu(mainMenu, len(mainMenu));
    radioStatsMenuVisible = false;
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

  if (lastPos != currentPos) {
    lastPos = currentPos;
    lcd.setCursor(0, (currentPos + 2) % 2);
    lcd.write(rightArrow); 
    lcd.setCursor(0, (currentPos + 1) % 2);
    lcd.print(" ");
  }
}


//Display Functions

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

void displayRadioStats() {
  if (!radioStatsMenuVisible) {
    radioStatsMenuVisible = true;
    lcd.clear();
  }
  if(millis() - lastMillisUpdate > 1000) {
    lastMillisUpdate = millis();
    lcd.setCursor(0, 0);
    lcd.print("S ");
    printInt(sentPacketsDisplay, 3);
    lcd.setCursor(7, 0);
    lcd.print("R ");
    printInt(receivedPacketsDisplay, 3);
    lcd.setCursor(0, 1);
    lcd.print("A ");
    printInt(ackedPacketsDisplay, 3);
  }
}

void updateMenu() {
  if (currentMenu == backgroundMenu) {
    displayBackground();
  } else if (currentMenu == radioStatsMenu) {
    displayRadioStats();
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
  } else if (currentMenu == expoMenu) {
    expoMenuCallback();
  } else if (currentMenu == invertMenu) {
    invertMenuCallback();
  } else if (currentMenu == mixingMenu) {
    mixingMenuCallback();
  } else if (currentMenu == saveMenu) {
    saveMenuCallback();
  } else if (currentMenu == radioStatsMenu) {
    radioStatsMenuCallback();
  }
}

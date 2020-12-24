#ifndef MENU_H
#define MENU_H

#include "Arduino.h"
#include "Button.h"
#include "LCD.h"
#include "AnalogChannel.h"

#define txLowBattThreshold 7.0
#define rxLowBattThreshold 10.5

#define trimLowerLimit -30
#define trimUpperLimit 30

#define f(x) (reinterpret_cast<const __FlashStringHelper*>(&x[0]))
#define len(x) (sizeof(x)/sizeof(x[0]))
typedef const __FlashStringHelper* fstring;
typedef const char PROGMEM pchar;
//Menus
extern pchar Display[];
extern pchar Trim[];
extern pchar Range[];
extern pchar Invert[];
extern pchar Save[];
extern pchar RadioStats[];

extern fstring mainMenu[];
extern fstring backgroundMenu[];

extern pchar Ch1[];
extern pchar Ch2[];
extern pchar Ch3[];
extern pchar Ch4[];

extern fstring displayMenu[];
extern fstring trimMenu[];
extern fstring rangeMenu[];
extern fstring invertMenu[];

extern pchar saveMsg[];
extern pchar calibrateMsg[];
extern pchar clearEEPROMMsg[];
extern fstring saveMenu[];

extern fstring radioStatsMenu[];

extern fstring* subMenus[];
extern byte subMenuSizes[];

/*--------------------------------*/

//Variables
extern fstring* currentMenu;
extern byte currentPos, lastPos;
extern byte currentMenuSize;
extern byte lastScreen;
extern bool optionSelected;
extern bool backgroundVisible, radioStatsMenuVisible;

extern AnalogChannel channel[4];

extern byte packetSucccessRate;
extern float transmitterVoltage, receiverVoltage;
extern unsigned int sentPacketsDisplay, receivedPacketsDisplay, ackedPacketsDisplay;
//

void updateMenu();
void initMenu();

#endif
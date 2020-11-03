#include "Globals.h"
#include "AnalogChannel.h"
#include "Menu.h"
#include <RF24.h>

void setup() {
  channel[0].begin(A0, 0*blockSize, THROTTLE);
  channel[1].begin(A1, 1*blockSize, SERVO);
  channel[2].begin(A2, 2*blockSize, SERVO);
  channel[3].begin(A3, 3*blockSize, SERVO);

  for (byte i = 0; i < 4; i++) {
    channel[i].load();
  }
  
  initMenu();
}

void loop() {

  for (byte i = 0; i < 4; i++){
    channel[i].update();
  }
  
  updateMenu();
}

#include "Init.h"
#include "Menu.h"
#include "Mixing.h"
#include "Radio.h"

void setup() { initAll(); }

void loop() {
  static unsigned long lastMillis = millis();
  static unsigned int sentPackets, receivedPackets, ackedPackets;

  for (byte i = 0; i < 4; i++) {
    txData.channel[i] = channel[i].update();
  }
  applyModelSettings();

  if (moduleConnected && throttleCheck()) {
    if (!receiverConnected) {
      if (beginCommunications())
        receiverConnected = true;
    } else {
      radio.write(&txData, sizeof(txData));
      sentPackets++;

      if (radio.available()) {
        radio.read(&rxData, sizeof(rxData));
        receivedPackets = rxData.pps;
        ackedPackets++;
      }
    }
  }

  if (millis() - lastMillis > 1000) {
    packetSucccessRate =
        sentPackets > 0 ? (ackedPackets * 100) / sentPackets : 0;

#ifdef SERIAL_DEBUG
    if (receiverConnected)
      printf("Sent = %d, Received = %d, Acked = %d\n", sentPackets,
             receivedPackets, ackedPackets);
#endif

    sentPacketsDisplay = sentPackets;
    receivedPacketsDisplay = receivedPackets;
    ackedPacketsDisplay = ackedPackets;
    sentPackets = 0;
    receivedPackets = 0;
    ackedPackets = 0;
    lastMillis = millis();
  }

  if (throttleCheck())
    updateMenu();
}

bool throttleCheck() {
  static bool throttleCheckPassed = false;

  if (!throttleCheckPassed) {
    if (txData.channel[0] == 0)
      throttleCheckPassed = true;
    else {
      static bool messageVisible;
      if (!messageVisible) {
        printfLCD(F("Throttle High"), F("Continue?"));
        messageVisible = true;
      }
      backgroundVisible = false; // To ensure the background shows up again
                                 // later
      if (getButtonState() == RIGHT)
        throttleCheckPassed = true;
    }
  }
  return throttleCheckPassed;
}
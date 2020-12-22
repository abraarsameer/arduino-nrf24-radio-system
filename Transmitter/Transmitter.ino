#include "Globals.h"
#include "AnalogChannel.h"
#include "Menu.h"

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

#define SERIAL_DEBUG

#define CE_PIN 9
#define CS_PIN 10

RF24 radio(CE_PIN, CS_PIN);
const uint64_t pipe = 0xABCDABCD71LL; 


struct ChannelData {
  byte channel[4];
} txData;

struct TelemetryData {
  byte batt;
  unsigned int pps;
} rxData;

void setup() {
  //Initialize LCD Menu
  initMenu();

  //Serial initialization
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
  Serial.println(F("RC Transmitter Serial Debug"));
  printf_begin();
#endif

  //Channel initialization
  channel[0].begin(A0, 0*blockSize, THROTTLE);
  channel[1].begin(A1, 1*blockSize, SERVO);
  channel[2].begin(A2, 2*blockSize, SERVO);
  channel[3].begin(A3, 3*blockSize, SERVO);

  for (byte i = 0; i < 4; i++) {
    channel[i].load();
  }

  //Radio initialization code
  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.enableDynamicPayloads();
  radio.enableAckPayload(); //Enable payload with Ack bit
  radio.setRetries(2, 0);
  radio.openWritingPipe(pipe);
  radio.stopListening();

  if(!radio.isChipConnected()) {
    lcd.clear();
    lcd.print(F("NRF24 not found"));
    delay(1000);
    backgroundVisible = false;
  }
  
  memset(&txData, 0, sizeof(txData));
  memset(&rxData, 0, sizeof(rxData));
}

void loop() {
  static unsigned long lastMillis = millis();
  static unsigned int sentPackets, receivedPackets, ackedPackets;

  for (byte i = 0; i < 4; i++) {
    txData.channel[i] = channel[i].update();
  }

  radio.write(&txData, sizeof(txData));
  sentPackets++;

 
  if (radio.available()) {
    radio.read(&rxData, sizeof(rxData));
    receivedPackets = rxData.pps;
    ackedPackets++;
  }
  
  if(millis() - lastMillis > 1000) {
    receiverVoltage = (rxData.batt/255.0)*5.0;
    packetSucccessRate = sentPackets > 0 ? (ackedPackets * 100) / sentPackets : 0;

#ifdef SERIAL_DEBUG
    printf("Sent = %d, Received = %d, Acked = %d\n", sentPackets, receivedPackets, ackedPackets);
#endif
    sentPacketsDisplay = sentPackets;
    receivedPacketsDisplay = receivedPackets;
    ackedPacketsDisplay = ackedPackets;
    sentPackets = 0;
    receivedPackets = 0;
    ackedPackets = 0;
    lastMillis = millis();
  }

  updateMenu();
}

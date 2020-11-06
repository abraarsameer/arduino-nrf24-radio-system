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
  channels[4];
} txData;

struct TelemetryData {
  byte batt;
  unsigned int pps;
} rxData;

void setup() {
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
  radio.setPayloadSize(sizeof(txData));
  radio.setAutoAck(true);
  radio.enableAckPayload(); //Enable payload with Ack bit
  //radio.setRetries(0, 15);
  radio.openWritingPipe(pipe);
  
  memset(txData, 0, sizeof(txData));
  memset(rxData, 0, sizeof(rxData));
  
  initMenu();
}

void loop() {

  for (byte i = 0; i < 4; i++){
    channel[i].update();
  }
  
  updateMenu();
}

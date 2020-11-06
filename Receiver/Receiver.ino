#include <RF24.h>
#include <Servo.h>

#define CE_PIN 7
#define CS_PIN 8

struct ChannelData {
  byte channel[4];
} txData;

struct TelemetryData {
  byte batt;
  unsigned int pps;
} rxData;

byte chPins[4] = {A5, A4, A3, A2};
Servo channel[4];

RF24 radio(CE_PIN, CS_PIN);
const uint64_t pipe = 0xABCDABCD71LL;

void setup() {
    for(byte i = 0; i < 3; i++) {
        channel[i].attach(chPins[i]);
    }

    radio.begin();
    radio.setDataRate(RF24_250KBPS);
    radio.setAutoAck(true);
    radio.enableAckPayload();
    radio.setPayloadSize(sizeof(rxData));
    radio.openReadingPipe(1, pipe);
    radio.startListening();

    memset(&txData, 0, sizeof(txData));
    memset(&rxData, 0, sizeof(rxData));
}

void loop() {
    
}
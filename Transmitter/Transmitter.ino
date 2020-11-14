#include "Globals.h"
#include "AnalogChannel.h"
#include "Menu.h"

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//#define SERIAL_DEBUG

#define CE_PIN 9
#define CS_PIN 10

RF24 radio(CE_PIN, CS_PIN);
const uint64_t pipe = 0xABCDABCD71LL;
bool moduleConnected, receiverConnected;

struct ChannelData
{
  byte channel[4];
  bool isFailsafeState : 1;
} txData;

struct TelemetryData
{
  byte batt;
  unsigned int pps : 12;
  bool outputEnabled : 1;
} rxData;

void setup()
{
  //Initialize LCD Menu
  initMenu();

  //Serial initialization
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
  Serial.println(F("RC Transmitter Serial Debug"));
  printf_begin();
#endif

  //Channel initialization
  channel[0].begin(A0, 0 * blockSize, THROTTLE);
  channel[1].begin(A1, 1 * blockSize, SERVO);
  channel[2].begin(A2, 2 * blockSize, SERVO);
  channel[3].begin(A3, 3 * blockSize, SERVO);

  for (byte i = 0; i < 4; i++)
  {
    channel[i].load();
  }

  //Radio initialization code
  radio.begin();
  moduleConnected = radio.isChipConnected();
  if (!moduleConnected)
  {
    printfLCD(F("NRF24 not found"));
    delay(1000);
  }
  else
  {
    radio.setDataRate(RF24_250KBPS);
    radio.setPayloadSize(sizeof(txData));
    radio.enableAckPayload(); //Enable payload with Ack bit
    radio.openWritingPipe(pipe);
    radio.setRetries(2, 0);
  }

  memset(&rxData, 0, sizeof(rxData));

  //Update channels for a while to stablize
  unsigned long lastMillis = millis();
  while (millis() - lastMillis < 500)
  {
    for (byte i = 0; i < 4; i++)
      channel[i].update();
  }
}

void loop()
{
  static unsigned long lastMillis = millis();
  static unsigned int sentPackets, receivedPackets, ackedPackets;

  for (byte i = 0; i < 4; i++)
  {
    txData.channel[i] = channel[i].update();
  }

  if (moduleConnected)
  {
    if (!receiverConnected)
    {
      if (beginCommunications())
        receiverConnected = true;
    }
    else
    {
      radio.write(&txData, sizeof(txData));
      sentPackets++;

      while (radio.isAckPayloadAvailable())
      {
        radio.read(&rxData, sizeof(rxData));
        receivedPackets = rxData.pps;
        ackedPackets++;
      }
    }
  }

  if (millis() - lastMillis > 1000)
  {
    receiverVoltage = (rxData.batt / 255.0) * 5.0;
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

bool beginCommunications()
{
  if (txData.channel[0] == 0) //Verify that throttle is set to 0 (other channels should be centered)
  {
    radio.setRetries(2, 15);
    txData.isFailsafeState = true;
    if (radio.write(&txData, sizeof(txData)))
    {
      txData.isFailsafeState = false;
      radio.setRetries(2, 0);
      return true;
    }
  }

  return false;
}
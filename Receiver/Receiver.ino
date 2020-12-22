//#define SERIAL_DEBUG

#include <RF24.h>
#include <Servo.h>

#define CE_PIN 7
#define CS_PIN 8

struct ChannelData
{
    byte channel[4];
} txData;

struct TelemetryData
{
    byte batt;
    unsigned int pps;
} rxData;

byte chPins[4] = {A5, A4, A3, A2};
Servo channel[4];

RF24 radio(CE_PIN, CS_PIN);
const uint8_t address[] = "tx1"; //Size should be within 3-5 characters

unsigned int receivedPackets;

void setup()
{
    for (byte i = 0; i < 4; i++)
    {
        channel[i].attach(chPins[i]);
    }

    radio.begin();
    radio.setDataRate(RF24_250KBPS);
    radio.enableDynamicPayloads();
    radio.enableAckPayload();
    radio.setAddressWidth(sizeof(address) - 1);
    radio.openReadingPipe(1, address);
    radio.startListening();

    memset(&txData, 90, sizeof(txData));
    memset(&rxData, 0, sizeof(rxData));

#ifdef SERIAL_DEBUG
    Serial.begin(115200);
    Serial.println(F("Receiver Serial Debug"));
#endif
}

void loop()
{
    static unsigned long lastMillis = millis();

    if (radio.available())
    {
        radio.writeAckPayload(1, &rxData, sizeof(rxData));
        radio.read(&txData, sizeof(txData));
        receivedPackets++;
    }

    if (millis() - lastMillis > 1000)
    {
        rxData.batt = 0;
        rxData.pps = receivedPackets;

        receivedPackets = 0;
        lastMillis = millis();
#ifdef SERIAL_DEBUG
        Serial.print(F("Received = "));
        Serial.println(rxData.pps);
#endif
    }

    for (byte i = 0; i < 4; i++)
    {
        channel[i].write(txData.channel[i]);
    }
}
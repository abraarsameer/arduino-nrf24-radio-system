#include <RF24.h>
#include <Servo.h>

#define CE_PIN 7
#define CS_PIN 8

#define FAILSAFE_DELAY 500
#define FAILSAFE_THRESHOLD 6

struct ChannelData
{
    byte channel[4];
    bool isFailsafeState : 1;
} txData, failsafeState;

struct TelemetryData
{
    byte batt;
    unsigned int pps : 12;
    bool outputEnabled : 1;
} rxData;

byte chPins[4] = {A5, A4, A3, A2};
Servo channel[4];

RF24 radio(CE_PIN, CS_PIN);
const uint64_t pipe = 0xABCDABCD71LL;

unsigned int receivedPackets, receivedPacketsFailsafe;

bool outputEnabled;

void setup()
{
    for (byte i = 0; i < 4; i++)
    {
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

void loop()
{
    while (radio.available())
    {
        radio.writeAckPayload(1, &rxData, sizeof(rxData));
        radio.read(&txData, sizeof(txData));
        receivedPackets++;
        receivedPacketsFailsafe++;
    }

    parseTxData();
    updateFailsafe();

    //Write outputs
    if (outputEnabled)
    {
        for (byte i = 0; i < 4; i++)
        {
            channel[i].write(txData.channel[i]);
        }
    }

    //Execute every 1000ms
    static unsigned long lastMillis = millis();
    if (millis() - lastMillis > 1000)
    {
        rxData.batt = 0;
        rxData.pps = receivedPackets;

        receivedPackets = 0;
        lastMillis = millis();
    }
}

byte readBatt()
{
    unsigned int val = 0;

    for (byte i = 0; i < 10; i++)
    {
        val += analogRead(A0);
    }

    val = (val / 10) >> 2;

    return val;
}

void updateFailsafe()
{
    static unsigned long lastMillis = millis();
    static bool failsafeMode = false;

    if (failsafeMode)
        txData = failsafeState;

    if (millis() - lastMillis > FAILSAFE_DELAY)
    {
        if (receivedPacketsFailsafe <= FAILSAFE_THRESHOLD)
        {
            failsafeMode = true; //Enable Failsafe Mode
        }
        else
        {
            failsafeMode = false; //Return to normal state
        }
        receivedPacketsFailsafe = 0;
        lastMillis = millis();
    }
}

void parseTxData()
{
    if (txData.isFailsafeState && !outputEnabled)
    {
        failsafeState = txData;
        outputEnabled = true;
        rxData.outputEnabled = true;
    }
}
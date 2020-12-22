//#define SERIAL_DEBUG

#include <RF24.h>
#include <Servo.h>

#define CE_PIN 7
#define CS_PIN 8

#define FAILSAFE_DELAY 500
#define FAILSAFE_THRESHOLD 6

struct ChannelData
{
    byte channel[4];
    bool isFailsafeState;
} txData, failsafeState;

struct TelemetryData
{
    unsigned int pps ;
    bool outputEnabled ;
} rxData;

byte chPins[4] = {A5, A4, A3, A2};
Servo channel[4];

RF24 radio(CE_PIN, CS_PIN);
const uint8_t address[] = "tx1"; //Size should be within 3-5 characters

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
    radio.enableDynamicPayloads();
    radio.enableAckPayload();
    radio.setAddressWidth(sizeof(address) - 1);
    radio.openReadingPipe(1, address);
    radio.startListening();

    memset(&txData, 0, sizeof(txData));
    memset(&rxData, 0, sizeof(rxData));

#ifdef SERIAL_DEBUG
    Serial.begin(115200);
    Serial.println(F("Receiver Serial Debug"));
#endif
}

void loop()
{
    if (radio.available())
    {
        radio.writeAckPayload(1, &rxData, sizeof(rxData));
        radio.read(&txData, sizeof(txData));
        radio.flush_rx();
        receivedPackets++;
        receivedPacketsFailsafe++;
    }

    //Write outputs
    if (outputEnabled)
    {
        for (byte i = 1; i < 4; i++)
        {
            channel[i].write(txData.channel[i]);
        }
        channel[0].writeMicroseconds(map(txData.channel[0], 0, 180, 1000, 2000));
    }

    parseTxData();
    updateFailsafe();

    //Execute every 1000ms
    static unsigned long lastMillis = millis();
    if (millis() - lastMillis > 1000)
    {
        rxData.pps = receivedPackets;

        receivedPackets = 0;
        lastMillis = millis();
#ifdef SERIAL_DEBUG
        Serial.print(F("Received = "));
        Serial.println(rxData.pps);
#endif
    }
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
        Serial.println(F("Got failsafe data"));
    }
}
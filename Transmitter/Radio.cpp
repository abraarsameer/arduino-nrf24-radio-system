#include "Radio.h"

RF24 radio(CE_PIN, CS_PIN);
const uint8_t address[] = "tx1";

bool moduleConnected, receiverConnected;

ChannelData txData;
TelemetryData rxData;

void initRadio() {
  // Radio initialization code
  radio.begin();
  moduleConnected = radio.isChipConnected();
  if (!moduleConnected) {
    printfLCD(F("NRF24 not found"));
    delay(1000);
  } else {
    radio.setDataRate(RF24_250KBPS);
    radio.enableDynamicPayloads();
    radio.enableAckPayload(); // Enable payload with Ack bit
    radio.setRetries(2, 0);
    radio.setAddressWidth(sizeof(address) - 1);
    radio.openWritingPipe(address);
    radio.stopListening();
  }
}

bool beginCommunications() {
  radio.setRetries(2, 15);
  txData.isFailsafeState = true;

  txData.channel[0] = 0;

  if (radio.write(&txData, sizeof(txData))) {
    txData.isFailsafeState = false;
    radio.setRetries(2, 0);

#ifdef SERIAL_DEBUG
    Serial.println(F("Connection success"));
#endif

    return true;
  }

  return false;
}
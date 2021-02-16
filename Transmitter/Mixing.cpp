#include "Mixing.h"

struct ModelConfig modelConfig;

static inline void applyMixes() {
  if (modelConfig.elevonMixEnabled) {
    int8_t elevator = txData.channel[2] - 90;
    int8_t aileron = txData.channel[3] - 90;

    txData.channel[2] = constrain(elevator + aileron, -90, 90) + 90;
    txData.channel[3] = constrain(elevator - aileron, -90, 90) + 90;
  }
}

static inline int8_t expo(int8_t x, byte k) {
  int8_t y = (k == 0) ? x
                      : (k * x * x * x / (HIGH_END_LIMIT * HIGH_END_LIMIT) +
                         x * (100 - k)) /
                            100;
  y = constrain(y, LOW_END_LIMIT, HIGH_END_LIMIT);
  return y;
}

void applyModelSettings() {
  // For all channels except throttle
  for (byte i = 1; i < 4; i++) {
    int val = expo(txData.channel[i], modelConfig.expo[i]);

    if (txData.channel[i] >= 0) {
      val = map(val, 0, HIGH_END_LIMIT, 0, modelConfig.highEndpoint[i]);
    } else {
      val = map(val, LOW_END_LIMIT, 0, modelConfig.lowEndpoint[i], 0);
    }

    val = val * modelConfig.secondaryRate[i] / 100 + modelConfig.trim[i];
    txData.channel[i] = constrain(val + 90, 0, 180);
  }

  // Throttle channel
  int val = map(txData.channel[0], LOW_END_LIMIT, HIGH_END_LIMIT, 0,
                2 * modelConfig.highEndpoint[0]);
  val = val * modelConfig.secondaryRate[0] / 100 + modelConfig.trim[0];
  val = constrain(val, 0, 180);

  txData.channel[0] = val;

  applyMixes();
}
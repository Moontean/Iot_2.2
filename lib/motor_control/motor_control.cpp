#include "motor_control.h"

namespace motor {

static Config gCfg;
static int16_t gPower = 0; // -100..100

static uint8_t percentToDuty(int16_t p) {
  // map absolute percent 0..100 to 0..255
  p = abs(p);
  if (p <= 0) return 0;
  if (p >= 100) return 255;
  return (uint8_t)((p * 255) / 100);
}

static void initDefaults() {
  gCfg.pwmPin = 255;
  gCfg.in1 = 255;
  gCfg.in2 = 255;
  gCfg.activeHigh = true;
}

void apply() {
  if (gCfg.pwmPin == 255) return;
  int16_t p = gPower;
  if (p == 0) {
    // ensure outputs are low to stop motor
    digitalWrite(gCfg.in1, LOW);
    digitalWrite(gCfg.in2, LOW);
    analogWrite(gCfg.pwmPin, 0);
    return;
  }
  uint8_t duty = percentToDuty(p);
  if (p > 0) {
    digitalWrite(gCfg.in1, gCfg.activeHigh ? HIGH : LOW);
    digitalWrite(gCfg.in2, gCfg.activeHigh ? LOW : HIGH);
  } else {
    digitalWrite(gCfg.in1, gCfg.activeHigh ? LOW : HIGH);
    digitalWrite(gCfg.in2, gCfg.activeHigh ? HIGH : LOW);
  }
  analogWrite(gCfg.pwmPin, duty);
}

void init(const Config& cfg) {
  initDefaults();
  gCfg = cfg;
  pinMode(gCfg.pwmPin, OUTPUT);
  pinMode(gCfg.in1, OUTPUT);
  pinMode(gCfg.in2, OUTPUT);
  stop();
}

void setPower(int16_t percent) {
  if (percent > 100) percent = 100;
  if (percent < -100) percent = -100;
  gPower = percent;
  apply();
}

void stop() {
  gPower = 0;
  apply();
}

void inc() {
  if (gPower >= 0) {
    gPower += 10;
    if (gPower > 100) gPower = 100;
  } else {
    gPower += 10; // move toward 0
    if (gPower > 0) gPower = 0;
  }
  apply();
}

void dec() {
  if (gPower <= 0) {
    gPower -= 10;
    if (gPower < -100) gPower = -100;
  } else {
    gPower -= 10; // move toward 0
    if (gPower < 0) gPower = 0;
  }
  apply();
}

void setMax() {
  if (gPower >= 0) gPower = 100;
  else gPower = -100;
  apply();
}

int16_t getPower() { return gPower; }

} // namespace motor

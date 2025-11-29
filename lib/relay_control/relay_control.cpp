#include "relay_control.h"

namespace relay {
static Config gCfg{255, true};
static bool gState = false;

void init(const Config& cfg) {
  gCfg = cfg;
  pinMode(gCfg.pin, OUTPUT);
  off();
}

void apply() {
  if (gCfg.pin == 255) return;
  const bool level = gState ? gCfg.activeHigh : !gCfg.activeHigh;
  digitalWrite(gCfg.pin, level ? HIGH : LOW);
}

void on() {
  gState = true;
  apply();
}

void off() {
  gState = false;
  apply();
}

bool isOn() { return gState; }

}

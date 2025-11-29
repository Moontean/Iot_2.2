#pragma once
#include <Arduino.h>

// Simple relay control API
namespace relay {

struct Config {
  uint8_t pin;
  bool activeHigh; // true if HIGH turns relay ON
};

void init(const Config& cfg);
void on();
void off();
bool isOn();

}

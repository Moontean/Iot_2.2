#pragma once
#include <Arduino.h>

namespace motor {

struct Config {
  uint8_t pwmPin;
  uint8_t in1;
  uint8_t in2;
  bool activeHigh = true;
};

// Initialize motor driver (L298 style)
void init(const Config& cfg);

// Set power -100..100 (%). Negative for reverse.
void setPower(int16_t percent);
void stop();
void inc();
void dec();
void setMax();

int16_t getPower();

} // namespace motor

#pragma once
#include <Arduino.h>

namespace kpad {

void init(uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4,
          uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4);

// Returns last key pressed or 0 if none; handles actions A/B/D internally if desired.
char poll();

}

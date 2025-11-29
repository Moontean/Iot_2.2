#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

namespace lcd {

void init(uint8_t addr = 0x27, uint8_t cols = 16, uint8_t rows = 2);
void printLine(uint8_t row, const char* text);
void clear();

}

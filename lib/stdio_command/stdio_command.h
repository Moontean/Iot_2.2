#pragma once
#include <Arduino.h>

namespace cmd {

// Initializes Serial for STDIO command input.
void init(unsigned long baud = 115200);

// Poll input and execute commands; returns true if a command was handled.
bool poll();

}

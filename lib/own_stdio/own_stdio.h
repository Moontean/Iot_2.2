#ifndef OWN_STDIO_H // Header guard for custom stdio
#define OWN_STDIO_H // Define header guard macro

#include <Arduino.h> // Arduino core for Serial
#include <stdio.h> // Standard I/O definitions
#include <LiquidCrystal_I2C.h> // LCD library for I2C display
#include <config.hpp> // Project configuration macros (LCD address, size)
#include <Keypad.h> // Keypad library for matrix keyboard

void own_stdio_init(const uint32_t baudrate); // Initialize custom stdio (Serial + LCD + keypad)

// Non-blocking keypad poll: returns key char or 0 if none
char keypad_peek();

#endif //OWN_STDIO_H // End of header guard
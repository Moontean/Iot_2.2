#include <Arduino.h>
#include <relay_control.h>
#include <lcd_display.h>
#include <stdio_command.h>
#include <keypad_input.h>

static const unsigned long BAUDRATE = 115200;

void setup() // Arduino setup function - runs once at startup
{ // Opening brace for setup function
  // Initialize relay, LCD, and STDIO command interface for lab 4.1
  relay::init({22, true}); // D22, activeHigh
  lcd::init(0x27, 16, 2);
  cmd::init(BAUDRATE); // Initializes Serial
  kpad::init(23,24,25,26, 27,28,29,30); // rows R1-R4, cols C1-C4

  // Print system initialization banner
  printf("\n");
  printf("======================================================\n");
  printf("   RELAY CONTROL SYSTEM (Lab 4.1)\n");
  printf("======================================================\n");
  printf("Features:\n");
  printf("- Relay ON/OFF via STDIO commands\n");
  printf("- LCD status display (I2C)\n");
  printf("- Modular hardware-software interfaces\n");
  printf("======================================================\n");
  printf("Assignment 4.1 focus: relay control & STDIO/LCD reporting\n");
  printf("======================================================\n\n");

  // Minimal loop-based command processing for 4.1 (no RTOS required)
  lcd::printLine(0, "Relay Control");
  lcd::printLine(1, relay::isOn() ? "Relay: ON " : "Relay: OFF");
} // Closing brace for setup function

void loop() // Arduino main loop function - normally runs continuously
{ // Opening brace for loop function
  // Poll Serial for commands and update LCD status
  if (cmd::poll()) {
    lcd::printLine(1, relay::isOn() ? "Relay: ON " : "Relay: OFF");
  }
  char key = kpad::poll();
  if (key) {
    if (key == 'A') {
      relay::on();
      lcd::printLine(1, "Relay: ON ");
      Serial.println("OK: relay=ON (keypad)");
    }
    else if (key == 'B') {
      relay::off();
      lcd::printLine(1, "Relay: OFF");
      Serial.println("OK: relay=OFF (keypad)");
    }
    else if (key == 'D') {
      lcd::printLine(1, relay::isOn() ? "Relay: ON " : "Relay: OFF");
      Serial.print("STATUS: relay=");
      Serial.println(relay::isOn() ? "ON" : "OFF");
    }
  }
} // Closing brace for loop function
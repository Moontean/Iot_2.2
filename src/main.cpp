// Motor control application for Lab 4.2
#include <Arduino.h>
#include "own_stdio.h"
#include "motor_control.h"

static const unsigned long BAUD = 115200;

// Default pins for Arduino Mega (can be changed)
static const uint8_t MOTOR_PWM = 6; // PWM capable
static const uint8_t MOTOR_IN1 = 7;
static const uint8_t MOTOR_IN2 = 8;

// Reporting interval (ms)
static const unsigned long REPORT_MS = 1000;

static unsigned long lastReport = 0;

// simple line buffer for Serial input
static String lineBuf;

void printStatus() {
  int16_t p = motor::getPower();
  const char* dir = (p > 0) ? "FWD" : (p < 0) ? "REV" : "STOP";
  printf("MOTOR: power=%d%% dir=%s\n", p, dir);
}

void handleCommand(const String &s) {
  String cmd = s;
  cmd.trim();
  cmd.toLowerCase();
  if (cmd.startsWith("motor set")) {
    int val = cmd.substring(9).toInt();
    motor::setPower(val);
    printf("OK: motor set %d\n", val);
    printStatus();
  } else if (cmd == "motor stop") {
    motor::stop();
    printf("OK: motor stop\n");
    printStatus();
  } else if (cmd == "motor max") {
    motor::setMax();
    printf("OK: motor max\n");
    printStatus();
  } else if (cmd == "motor inc") {
    motor::inc();
    printf("OK: motor inc\n");
    printStatus();
  } else if (cmd == "motor dec") {
    motor::dec();
    printf("OK: motor dec\n");
    printStatus();
  } else if (cmd == "status" || cmd == "motor status") {
    printStatus();
  } else {
    printf("ERR: unknown command: %s\n", cmd.c_str());
  }
}

void setup() {
  own_stdio_init(BAUD);
  motor::Config cfg;
  cfg.pwmPin = MOTOR_PWM;
  cfg.in1 = MOTOR_IN1;
  cfg.in2 = MOTOR_IN2;
  cfg.activeHigh = true;
  motor::init(cfg);

  printf("\n==============================================\n");
  printf(" Motor control (Lab 4.2)\n");
  printf(" Commands: motor set <n>, motor stop, motor max, motor inc, motor dec, status\n");
  printf("==============================================\n");

  lastReport = millis();
}

void loop() {
  // Serial input line handling
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\r' || c == '\n') {
      if (lineBuf.length() > 0) {
        handleCommand(lineBuf);
        lineBuf = "";
      }
    } else {
      lineBuf += c;
      if (lineBuf.length() > 64) lineBuf = lineBuf.substring(0,64);
    }
  }

  // keypad support (non-blocking)
  char k = keypad_peek();
  if (k) {
    // simple mapping: A -> inc, B -> dec, D -> status, digits -> set immediate
    if (k == 'A') { motor::inc(); printf("OK: motor inc (key)\n"); }
    else if (k == 'B') { motor::dec(); printf("OK: motor dec (key)\n"); }
    else if (k == 'D') { printStatus(); }
    else if (k >= '0' && k <= '9') {
      // read possible multi-digit: build small number via blocking keypad_getchar
      int value = k - '0';
      // try to read another key (simple, not robust)
      char k2 = keypad_peek();
      if (k2 >= '0' && k2 <= '9') {
        value = value * 10 + (k2 - '0');
      }
      motor::setPower(value);
      printf("OK: motor set %d (key)\n", value);
      printStatus();
    }
  }

  // periodic report
  unsigned long now = millis();
  if (now - lastReport >= REPORT_MS) {
    printStatus();
    lastReport = now;
  }
}
#include "stdio_command.h"
#include <relay_control.h>

namespace cmd {
static String buffer;

void init(unsigned long baud) {
  Serial.begin(baud);
  buffer.reserve(64);
  Serial.println("CMD: ready. Use 'relay on', 'relay off', or keys A/B/D.");
}

static void handleLine(const String& line) {
  String s = line;
  s.trim();
  s.toLowerCase();
  if (s == "relay on" || s == "a") {
    relay::on();
    Serial.println("OK: relay=ON");
  } else if (s == "relay off" || s == "b") {
    relay::off();
    Serial.println("OK: relay=OFF");
  } else if (s == "status" || s == "d") {
    Serial.print("STATUS: relay=");
    Serial.println(relay::isOn() ? "ON" : "OFF");
  } else {
    Serial.print("ERR: unknown command: ");
    Serial.println(s);
  }
}

bool poll() {
  bool handled = false;
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    // Handle both CR and LF as end of line
    if (c == '\r' || c == '\n') {
      if (buffer.length() > 0) {
        handleLine(buffer);
        buffer = "";
        handled = true;
      }
    } else {
      buffer += c;
      if (buffer.length() > 60) {
        buffer = buffer.substring(0, 60);
      }
    }
  }
  return handled;
}

}

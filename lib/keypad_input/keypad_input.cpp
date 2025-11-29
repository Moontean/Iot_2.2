#include "keypad_input.h"
#include <Keypad.h>

namespace kpad {
static const byte ROWS = 4;
static const byte COLS = 4;
static char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
static byte rowPins[ROWS];
static byte colPins[COLS];
static Keypad* keypad = nullptr;

void init(uint8_t r1, uint8_t r2, uint8_t r3, uint8_t r4,
          uint8_t c1, uint8_t c2, uint8_t c3, uint8_t c4) {
  rowPins[0]=r1; rowPins[1]=r2; rowPins[2]=r3; rowPins[3]=r4;
  colPins[0]=c1; colPins[1]=c2; colPins[2]=c3; colPins[3]=c4;
  keypad = new Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
}

char poll() {
  if (!keypad) return 0;
  char k = keypad->getKey();
  return k ? k : 0;
}

}

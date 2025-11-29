#include "lcd_display.h"

namespace lcd {
static LiquidCrystal_I2C lcd(0x27, 16, 2);
static uint8_t gCols = 16;

void init(uint8_t addr, uint8_t cols, uint8_t rows) {
  lcd = LiquidCrystal_I2C(addr, cols, rows);
  gCols = cols;
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void printLine(uint8_t row, const char* text) {
  lcd.setCursor(0, row);
  bool finished = false;
  for (uint8_t i = 0; i < gCols; ++i) {
    if (!finished) {
      char c = text[i];
      if (c == '\0') {
        finished = true;
        lcd.print(" ");
      } else {
        lcd.print(c);
      }
    } else {
      lcd.print(" ");
    }
  }
}

void clear() { lcd.clear(); }

}

#include "button_control.h"

void button_control_init(const uint8_t pin)
{
    pinMode(pin, INPUT_PULLUP);
}

uint8_t is_button_pressed(const uint8_t pin)
{
    return digitalRead(pin) == LOW;
}
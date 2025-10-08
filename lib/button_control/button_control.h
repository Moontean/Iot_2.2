#ifndef BUTTON_CONTROL_H
#define BUTTON_CONTROL_H

#include <Arduino.h>

void button_control_init(const uint8_t pin);
uint8_t is_button_pressed(const uint8_t pin);

#endif //BUTTON_CONTROL_H
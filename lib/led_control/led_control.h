#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <Arduino.h>

void led_control_init(const uint8_t pin);
void led_on(const uint8_t pin);
void led_off(const uint8_t pin);

#endif
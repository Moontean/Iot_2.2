#ifndef LED_CONTROL_H // Header guard for LED control
#define LED_CONTROL_H // Define the LED control header guard

#include <Arduino.h> // Include Arduino core functions

void led_control_init(const uint8_t pin); // Initialize LED pin (prepare output and default state)
void led_on(const uint8_t pin); // Set LED pin to HIGH (turn on)
void led_off(const uint8_t pin); // Set LED pin to LOW (turn off)
void led_toggle(const uint8_t pin); // Toggle LED pin state

#endif // End of header guard
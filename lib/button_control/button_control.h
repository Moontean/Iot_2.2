#ifndef BUTTON_CONTROL_H // Header guard to prevent multiple inclusion
#define BUTTON_CONTROL_H // Define the header guard macro

#include <Arduino.h> // Include Arduino core definitions

void button_control_init(const uint8_t pin); // Initialize a button pin with INPUT_PULLUP
uint8_t is_button_pressed(const uint8_t pin); // Return 1 if button is pressed (active-low)

#endif //BUTTON_CONTROL_H // End of header guard
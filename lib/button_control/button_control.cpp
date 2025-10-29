#include "button_control.h" // Include button control declarations

void button_control_init(const uint8_t pin) // Function to configure a button pin
{
    pinMode(pin, INPUT_PULLUP); // Set pin as input with internal pull-up resistor
}

uint8_t is_button_pressed(const uint8_t pin) // Function to read button state
{
    return digitalRead(pin) == LOW; // Return true when pin reads LOW (pressed)
}
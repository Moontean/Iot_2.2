#include <led_control.h> // Include LED control API

void led_control_init(const uint8_t pin) // Configure LED pin and set initial state
{
    pinMode(pin, OUTPUT); // Set pin mode to output
    digitalWrite(pin, LOW); // Ensure LED is off initially
}

void led_on(const uint8_t pin) // Turn LED on
{
    digitalWrite(pin, HIGH); // Drive pin HIGH
}

void led_off(const uint8_t pin) // Turn LED off
{
    digitalWrite(pin, LOW); // Drive pin LOW
}

void led_toggle(const uint8_t pin) // Toggle LED state
{
    digitalWrite(pin, !digitalRead(pin)); // Write inverse of current pin state
}
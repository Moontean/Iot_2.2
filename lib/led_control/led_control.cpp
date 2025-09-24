#include <led_control.h>

void led_control_init(const uint8_t pin)
{
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
}

void led_on(const uint8_t pin)
{
    digitalWrite(pin, HIGH);
}

void led_off(const uint8_t pin)
{
    digitalWrite(pin, LOW);
}
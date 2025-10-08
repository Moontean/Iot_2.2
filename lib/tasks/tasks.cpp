#include <tasks.h>

static uint8_t first_led_state;
static uint16_t second_task_frequency_ms;

void first_task_init(void)
{
    led_control_init(GREEN_LED_PIN);
    button_control_init(ON_OFF_BUTTON_PIN);
    first_led_state = HIGH;
    led_on(GREEN_LED_PIN);
}

void first_task(void* args)
{
    static uint8_t need_init = true;
    static uint32_t next_time_to_check_button = 0;

    if (need_init)
    {
        first_task_init();
        need_init = false;
    }

    if (is_button_pressed(ON_OFF_BUTTON_PIN))
    {
        if (millis() >= next_time_to_check_button)
        {
            first_led_state = !first_led_state;
            if (first_led_state)
            {
                led_on(GREEN_LED_PIN);
            }
            else
            {
                led_off(GREEN_LED_PIN);
            }
            next_time_to_check_button = millis() + DEBOUNCE_TIME_MS; //debounce time
        }
    }
}

void second_task_init(void)
{
    led_control_init(BLUE_LED_PIN);
    led_off(BLUE_LED_PIN);
    second_task_frequency_ms = DEFAULT_FREQUENCY_MS;
}

void second_task(void* args)
{
    static uint8_t need_init = true;
    static uint32_t next_toggle_time = 0;
    if (need_init)
    {
        second_task_init();
        need_init = false;
    }

    if (first_led_state == LOW)
    {
        if (millis() >= next_toggle_time)
        {
            led_toggle(BLUE_LED_PIN);
            next_toggle_time = millis() + second_task_frequency_ms;
        }
    }
}
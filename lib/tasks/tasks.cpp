#include <tasks.h>

static uint8_t first_led_state;

SemaphoreHandle_t button_led_semaphore;

void first_task_init(void)
{
    led_control_init(GREEN_LED_PIN);
    button_control_init(ON_OFF_BUTTON_PIN);
    first_led_state = LOW;
    led_on(GREEN_LED_PIN);

    if(button_led_semaphore == NULL)
    {
        button_led_semaphore = xSemaphoreCreateBinary();
    }
}

void first_task(void* args)
{
    static uint8_t need_init = true;
    static TickType_t next_time_to_check_button = 0;
    static TickType_t led_on_duration = 0;
    static TickType_t first_task_time = 0;
    if (need_init)
    {
        first_task_init();
        need_init = false;
    }
    for (;;)

        if (is_button_pressed(ON_OFF_BUTTON_PIN))
        {
            if (xTaskGetTickCount() > next_time_to_check_button)
            {
                led_toggle(GREEN_LED_PIN);
                next_time_to_check_button = xTaskGetTickCount() + DEBOUNCE_TIME_MS / portTICK_PERIOD_MS;

            }
        }
        vTaskDelayUntil(&first_task_time, FIRST_TASK_RECURRENCE_MS / portTICK_PERIOD_MS);
    }

void second_task_init(void)
{
    led_control_init(BLUE_LED_PIN);
    led_off(BLUE_LED_PIN);
}

void second_task(void* args)
{
    static uint8_t need_init = true;
    static uint32_t next_time_to_toggle_led = 0;
    static uint8_t N = 0;
    static uint8_t led_state = LOW;
    static uint8_t blink_count = 0;

    if (need_init)
    {
        second_task_init();
        need_init = false;
    }
    for (;;)
        if (xSemaphoreTake(button_led_semaphore, portMAX_DELAY) == pdTRUE)
        {
            N = N + 1;
            blink_count = N;
        }
        if  (blink_count > 0)
        {
            led_on(BLUE_LED_PIN);
            vTaskDelayUntil(NULL, SECOND_LED_ON_TIME_MS / portTICK_PERIOD_MS);
            led_off(BLUE_LED_PIN);
            vTaskDelayUntil(NULL, SECOND_LED_OFF_TIME_MS / portTICK_PERIOD_MS);
            blink_count--;
        }
        vTaskDelay(SECOND_TASK_RECURRENCE_MS / portTICK_PERIOD_MS);
}
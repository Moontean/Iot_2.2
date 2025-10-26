#include <tasks.h>

// Глобальные объекты FreeRTOS (определения)
QueueHandle_t gBytesQueue = nullptr;
SemaphoreHandle_t gButtonSemaphore = nullptr;
byte gN = 0;

// Task 1 — Кнопка и LED: период 10 мс, семафор по нажатию, LED на 1 сек
void task1_button_led(void* pvParameters)
{
    led_control_init(GREEN_LED_PIN);
    led_off(GREEN_LED_PIN);
    button_control_init(ON_OFF_BUTTON_PIN);

    TickType_t lastWake = xTaskGetTickCount();
    TickType_t ledDeadline = 0;
    TickType_t nextPressAllowed = 0;

    for (;;)
    {
        const TickType_t now = xTaskGetTickCount();
        const uint8_t pressed = is_button_pressed(ON_OFF_BUTTON_PIN);

        if (pressed && now >= nextPressAllowed)
        {
            xSemaphoreGive(gButtonSemaphore);
            ledDeadline = now + pdMS_TO_TICKS(1000);
            nextPressAllowed = now + pdMS_TO_TICKS(DEBOUNCE_TIME_MS);
        }

        if (ledDeadline && now < ledDeadline)
        {
            led_on(GREEN_LED_PIN);
        }
        else
        {
            led_off(GREEN_LED_PIN);
            ledDeadline = 0;
        }

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(TASK1_PERIOD_MS));
    }
}

// Task 2 — Синхронная: ждёт семафор, отправляет 1..N с интервалом 50 мс и мигает N раз
void task2_provider(void* pvParameters)
{
    led_control_init(BLUE_LED_PIN);
    led_off(BLUE_LED_PIN);

    for (;;)
    {
        if (xSemaphoreTake(gButtonSemaphore, portMAX_DELAY) == pdTRUE)
        {
            gN++;

            for (byte i = 1; i <= gN && i != 0; ++i)
            {
                byte v = i;
                xQueueSendToFront(gBytesQueue, &v, portMAX_DELAY);
                vTaskDelay(pdMS_TO_TICKS(TASK2_SEND_INTERVAL_MS));
            }

            byte zero = 0;
            xQueueSendToFront(gBytesQueue, &zero, portMAX_DELAY);

            for (uint32_t j = 0; j < gN; ++j)
            {
                led_on(BLUE_LED_PIN);
                vTaskDelay(pdMS_TO_TICKS(TASK2_LED_ON_MS));
                led_off(BLUE_LED_PIN);
                vTaskDelay(pdMS_TO_TICKS(TASK2_LED_OFF_MS));
            }
        }
    }
}

// Task 3 — Асинхронная: каждые 200 мс считывает очередь и печатает в терминал
void task3_consumer(void* pvParameters)
{
    TickType_t lastWake = xTaskGetTickCount();

    for (;;)
    {
        byte b;
        while (xQueueReceive(gBytesQueue, &b, 0) == pdPASS)
        {
            if (b == 0)
            {
                printf("\n");
            }
            else
            {
                printf("%i\r ",b);
            }
        }

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(TASK3_PERIOD_MS));
    }
}
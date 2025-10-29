#include <tasks.h>

// Глобальные объекты FreeRTOS (определения)


// Task 1 — Кнопка и LED: период 10 мс, семафор по нажатию, LED на 1 сек
void task1_button_led(void* pvParameters)
{
    TasksData* tasksData = (TasksData*)pvParameters;
    led_control_init(GREEN_LED_PIN);
    led_off(GREEN_LED_PIN);
    button_control_init(ON_OFF_BUTTON_PIN);
    // printf("Penis1\n");
    TickType_t lastWake = xTaskGetTickCount();
    TickType_t ledDeadline = 0;
    TickType_t nextPressAllowed = 0;

    for (;;)
    {
        const TickType_t now = xTaskGetTickCount();
        const uint8_t pressed = is_button_pressed(ON_OFF_BUTTON_PIN);

        if (pressed && now >= nextPressAllowed)
        {
            xSemaphoreGive(tasksData->gButtonSemaphore);
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
    TasksData* tasksData = (TasksData*)pvParameters;
    led_control_init(BLUE_LED_PIN);
    led_off(BLUE_LED_PIN);
    // printf("Penis2\n");
    byte v;
    byte zero = 0;
    for (;;)
    {
        if (xSemaphoreTake(tasksData->gButtonSemaphore, portMAX_DELAY) == pdTRUE)
        {
            tasksData->gN++;

            for (byte i = 1; i <= tasksData->gN; ++i)
            {
                // printf("Penis send:%d\n",i);
                v = i;
                xQueueSend(tasksData->gBytesQueue, &v, portMAX_DELAY);
                vTaskDelay(pdMS_TO_TICKS(TASK2_SEND_INTERVAL_MS));
            }

            xQueueSend(tasksData->gBytesQueue, &zero, portMAX_DELAY);

            for (uint32_t j = 0; j < tasksData->gN; ++j)
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
    TasksData* tasksData = (TasksData*)pvParameters;
    TickType_t lastWake = xTaskGetTickCount();
    // printf("Penis\n");
    for (;;)
    {
        byte b;
        while (xQueueReceive(tasksData->gBytesQueue, &b, 0) == pdTRUE)
        {
            if (b == 0)
            {
                // printf("Penis received zero, end of series.");
                printf("\n\r");
            }
            else
            {
                printf("%d ",b);
            }
        }

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(TASK3_PERIOD_MS));
    }
}

// Инициализация семафора, очереди и создание задач FreeRTOS
void rtos_tasks_init(void)
{
    static TasksData tasksData;
    tasksData.gN = 0;
    
    // Бинарный семафор: событие от Task1 к Task2
    tasksData.gButtonSemaphore = xSemaphoreCreateBinary();

    // Очередь сообщений: каждый элемент — ByteSeriesMsg
    // ВАЖНО: длину очереди НЕ меняем (берётся из существующего макроса/конфига)
    tasksData.gBytesQueue = xQueueCreate(QUEUE_LENGTH, sizeof(byte));

    // Создание задач
    xTaskCreate(task1_button_led, "Task1", 256, &tasksData, 1, nullptr);
    xTaskCreate(task2_provider,  "Task2", 256, &tasksData, 1, nullptr);
    xTaskCreate(task3_consumer,  "Task3", 256, &tasksData, 1, nullptr);
}

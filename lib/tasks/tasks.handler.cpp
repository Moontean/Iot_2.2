#include "tasks.h"

// Инициализация семафора, очереди и создание задач FreeRTOS
void rtos_tasks_init(void)
{
    // Бинарный семафор: событие от Task1 к Task2
    gButtonSemaphore = xSemaphoreCreateBinary();

    // Очередь сообщений: каждый элемент — ByteSeriesMsg
    // ВАЖНО: длину очереди НЕ меняем (берётся из существующего макроса/конфига)
    gBytesQueue = xQueueCreate(QUEUE_LENGTH, sizeof(byte));

    // Создание задач
    xTaskCreate(task1_button_led, "Task1", 256, nullptr, 2, nullptr);
    xTaskCreate(task2_provider,  "Task2", 256, nullptr, 2, nullptr);
    xTaskCreate(task3_consumer,  "Task3", 256, nullptr, 1, nullptr);
}

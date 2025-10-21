#include "tasks.h"

// Инициализация семафора, очереди и создание задач FreeRTOS
void rtos_tasks_init(void)
{
    // Бинарный семафор: событие от Task1 к Task2
    gButtonSemaphore = xSemaphoreCreateBinary();

    // Очередь: provider (Task2) → consumer (Task3), элементы — байты
    gBytesQueue = xQueueCreate(QUEUE_LENGTH, sizeof(uint8_t));

    // Создание задач (стек и приоритеты под Mega2560)
    xTaskCreate(task1_button_led, "Task1", 256, nullptr, 2, nullptr);
    xTaskCreate(task2_provider,  "Task2", 256, nullptr, 2, nullptr);
    xTaskCreate(task3_consumer,  "Task3", 256, nullptr, 1, nullptr);
}

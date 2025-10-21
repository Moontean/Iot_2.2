#include <Arduino.h>
#include <tasks.h>
#include <own_stdio.h>

void setup()
{
  Serial.begin(BAUDRATE);
  own_stdio_init(BAUDRATE);

  // Инициализируем RTOS-ресурсы и создаём задачи
  rtos_tasks_init();

  // Запуск планировщика FreeRTOS
  vTaskStartScheduler();
}

void loop()
{
  // При запущенном планировщике сюда, как правило, не возвращаемся
  // Empty loop as tasks are handled by timer interrupts
}
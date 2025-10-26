#ifndef TASKS_H
#define TASKS_H

#include <Arduino.h>
#include <led_control.h>
#include <button_control.h>
#include <config.hpp>
#include <timer_setup.h>

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

// stdio остаётся для вывода в Serial/LCD
#include <own_stdio.h>

// Глобальные объекты синхронизации и состояния (FreeRTOS)
extern QueueHandle_t gBytesQueue;
extern SemaphoreHandle_t gButtonSemaphore;
extern byte gN;
// Инициализация FreeRTOS задач/ресурсов
void rtos_tasks_init(void);

// FreeRTOS задачи
void task1_button_led(void* pvParameters);
void task2_provider(void* pvParameters);
void task3_consumer(void* pvParameters);
void tasks_update(void);

void first_task(void* args);
void second_task(void* args);
void third_task(void* args);
void idle_task(void* args);

#endif //TASKS_H
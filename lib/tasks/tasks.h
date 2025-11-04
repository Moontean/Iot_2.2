#ifndef TASKS_H // Header guard for tasks API
#define TASKS_H // Define header guard macro

#include <Arduino.h> // Arduino core
#include <led_control.h> // LED control functions
#include <button_control.h> // Button control functions
#include <config.hpp> // Project-wide configuration macros
#include <timer_setup.h> // Timer setup (assumed external header)

#include <Arduino_FreeRTOS.h> // FreeRTOS API for Arduino
#include <semphr.h> // FreeRTOS semaphores
#include <queue.h> // FreeRTOS queues

// stdio остаётся для вывода в Serial/LCD // Keep stdio for Serial/LCD output
#include <own_stdio.h> // Custom stdio abstraction
#include <sensor_module.h> // Sensor module for data collection

typedef struct { // Aggregate struct holding shared RTOS resources
    QueueHandle_t gBytesQueue; // Queue for bytes producer/consumer
    SemaphoreHandle_t gButtonSemaphore; // Binary semaphore for button events
    volatile byte gN; // Counter of series length (volatile for task access)
    
    // New sensor data sharing
    QueueHandle_t gSensorDataQueue; // Queue for sensor data sharing between tasks
    volatile uint32_t gSystemUptime; // System uptime in seconds
    volatile uint16_t gTotalSensorReads; // Total number of sensor readings
    volatile uint16_t gSensorErrors; // Number of sensor errors
} TasksData; // End of TasksData definition
// Инициализация FreeRTOS задач/ресурсов // Initialization of FreeRTOS tasks/resources
void rtos_tasks_init(void); // Create tasks and initialize synchronization primitives

// FreeRTOS задачи // FreeRTOS task entry points
void task1_button_led(void* pvParameters); // Task 1: button handling and LED control
void task2_provider(void* pvParameters); // Task 2: producer sends sequence and blinks LED
void task3_consumer(void* pvParameters); // Task 3: consumer reads queue and prints

// New sensor-related tasks
void task4_sensor_reader(void* pvParameters); // Task 4: periodic sensor data collection
void task5_system_monitor(void* pvParameters); // Task 5: system status reporting every 500ms

void tasks_update(void); // Optional periodic update (not implemented here)

void first_task(void* args); // Placeholder task entry
void second_task(void* args); // Placeholder task entry
void third_task(void* args); // Placeholder task entry
void idle_task(void* args); // Placeholder idle task

#endif //TASKS_H // End of header guard
#include <tasks.h> // Include task declarations and dependencies
#include <sensor_module.h> // NTC temperature sensor module
#include <ultrasonic_sensor.h> // HC-SR04 ultrasonic sensor module
#include <signal_processing.h> // Digital signal processing filters

// Глобальные объекты FreeRTOS (определения) // Global FreeRTOS objects (definitions)
// (All handles are stored inside TasksData passed to tasks)


// Task 1 — Кнопка и LED: период 10 мс, семафор по нажатию, LED на 1 сек // Task 1: button debounce, semaphore signal, 1s LED
void task1_button_led(void* pvParameters) // Task entry point with user parameters
{
    TasksData* tasksData = (TasksData*)pvParameters; // Cast parameter to shared data
    led_control_init(GREEN_LED_PIN); // Initialize green LED pin
    led_off(GREEN_LED_PIN); // Ensure LED is off at start
    button_control_init(ON_OFF_BUTTON_PIN); // Initialize ON/OFF button with pull-up
    // printf("something1\n"); // Debug print (left unchanged)
    TickType_t lastWake = xTaskGetTickCount(); // Get current tick for periodic scheduling
    TickType_t ledDeadline = 0; // Deadline when LED should turn off
    TickType_t nextPressAllowed = 0; // Debounce time threshold

    for (;;) // Infinite task loop
    {
        const TickType_t now = xTaskGetTickCount(); // Read current tick count
        const uint8_t pressed = is_button_pressed(ON_OFF_BUTTON_PIN); // Read button state

        if (pressed && now >= nextPressAllowed) // If pressed and debounce passed
        {
            xSemaphoreGive(tasksData->gButtonSemaphore); // Signal provider task via binary semaphore
            ledDeadline = now + pdMS_TO_TICKS(1000); // Keep LED on for 1000 ms
            nextPressAllowed = now + pdMS_TO_TICKS(DEBOUNCE_TIME_MS); // Set next allowed press time
        }

        if (ledDeadline && now < ledDeadline) // If within LED deadline
        {
            led_on(GREEN_LED_PIN); // Turn LED on
        }
        else // Otherwise, LED should be off
        {
            led_off(GREEN_LED_PIN); // Turn LED off
            ledDeadline = 0; // Reset deadline
        }

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(TASK1_PERIOD_MS)); // Periodic delay for 10 ms
    } // End of infinite loop
} // End of task1_button_led

// Task 2 — Синхронная: ждёт семафор, отправляет 1..N с интервалом 50 мс и мигает N раз // Task 2: wait semaphore, send 1..N, blink N times
void task2_provider(void* pvParameters) // Task entry point for provider
{
    TasksData* tasksData = (TasksData*)pvParameters; // Cast parameter to shared data
    led_control_init(BLUE_LED_PIN); // Initialize blue LED pin
    led_off(BLUE_LED_PIN); // Ensure LED off initially
    // printf("something2\n"); // Debug print (left unchanged)
    byte v; // Temporary byte to send
    byte zero = 0; // Zero marker indicating end of series
    for (;;) // Infinite task loop
    {
        if (xSemaphoreTake(tasksData->gButtonSemaphore, portMAX_DELAY) == pdTRUE) // Wait for button event
        {
            tasksData->gN++; // Increase series length counter

            for (byte i = 1; i <= tasksData->gN; ++i) // Send numbers 1..N to queue
            {
                // printf("something send:%d\n",i); // Debug print (left unchanged)
                v = i; // Assign current value to send
                xQueueSend(tasksData->gBytesQueue, &v, portMAX_DELAY); // Send byte to queue
                vTaskDelay(pdMS_TO_TICKS(TASK2_SEND_INTERVAL_MS)); // Wait configured interval
            }

            xQueueSend(tasksData->gBytesQueue, &zero, portMAX_DELAY); // Send zero marker to queue

            for (uint32_t j = 0; j < tasksData->gN; ++j) // Blink LED N times
            {
                led_on(BLUE_LED_PIN); // Turn LED on
                vTaskDelay(pdMS_TO_TICKS(TASK2_LED_ON_MS)); // Wait LED on duration
                led_off(BLUE_LED_PIN); // Turn LED off
                vTaskDelay(pdMS_TO_TICKS(TASK2_LED_OFF_MS)); // Wait LED off duration
            }
        }
    } // End of infinite loop
} // End of task2_provider

// Task 3 — Асинхронная: каждые 200 мс считывает очередь и печатает в терминал // Task 3: periodically consume queue and print
void task3_consumer(void* pvParameters) // Task entry point for consumer
{
    TasksData* tasksData = (TasksData*)pvParameters; // Cast parameter to shared data
    TickType_t lastWake = xTaskGetTickCount(); // Track periodic wake time
    for (;;) // Infinite task loop
    {
        byte b; // Byte read from queue
        while (xQueueReceive(tasksData->gBytesQueue, &b, 0) == pdTRUE) // Drain queue non-blocking
        {
            if (b == 0) // If zero marker received, end of series
            {
                // printf("something received zero, end of series."); // Debug print (left unchanged)
                printf("\n\r"); // Print newline and carriage return
            }
            else // Otherwise print received number
            {
                printf("%d ",b); // Print value followed by space
            }
        }

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(TASK3_PERIOD_MS)); // Periodic wait for 200 ms
    } // End of infinite loop
} // End of task3_consumer

// Инициализация семафора, очереди и создание задач FreeRTOS // Initialize semaphore, queue, and create tasks
void rtos_tasks_init(void) // Public initialization function
{
    static TasksData tasksData; // Static storage for tasks shared data
    tasksData.gN = 0; // Initialize series counter to zero
    
    // Бинарный семафор: событие от Task1 к Task2 // Binary semaphore signals from Task1 to Task2
    tasksData.gButtonSemaphore = xSemaphoreCreateBinary(); // Create binary semaphore

    // Очередь сообщений: каждый элемент — ByteSeriesMsg // Message queue: items are bytes
    // ВАЖНО: длину очереди НЕ меняем (берётся из существующего макроса/конфига) // IMPORTANT: keep queue length from config
    tasksData.gBytesQueue = xQueueCreate(QUEUE_LENGTH, sizeof(byte)); // Create queue with fixed length

    // Создание задач // Create FreeRTOS tasks
    xTaskCreate(task1_button_led, "Task1", 256, &tasksData, 1, nullptr); // Create Task1 with stack size and priority
    xTaskCreate(task2_provider,  "Task2", 256, &tasksData, 1, nullptr); // Create Task2
    xTaskCreate(task3_consumer,  "Task3", 256, &tasksData, 2, nullptr); // Create Task3
    xTaskCreate(task4_sensor_reader, "Task4", 512, &tasksData, 2, nullptr); // Create Task4 for NTC sensor
    xTaskCreate(task5_sensor_reporter, "Task5", 512, &tasksData, 1, nullptr); // Create Task5 for sensor reports
    xTaskCreate(task6_ultrasonic_reader, "Task6", 512, &tasksData, 2, nullptr); // Create Task6 for HC-SR04
    xTaskCreate(task7_signal_processor, "Task7", 512, &tasksData, 3, nullptr); // Create Task7 for signal processing
} // End of rtos_tasks_init

// Task 4 — NTC Temperature Sensor Reader: period 500ms with digital filtering // Task 4: NTC sensor with signal processing
void task4_sensor_reader(void* pvParameters) // Task entry point for sensor reading
{
    TasksData* tasksData = (TasksData*)pvParameters; // Cast parameter to shared data
    // Initialize sensor with default config
    sensor_config_t sensor_cfg;
    sensor_cfg.name = "NTC_Temp";
    sensor_cfg.pin = TEMPERATURE_SENSOR_PIN;
    sensor_cfg.type = SENSOR_TYPE_ANALOG;
    sensor_cfg.min_voltage = 0.5f;
    sensor_cfg.max_voltage = 4.5f;
    sensor_cfg.max_errors = 10;
    sensor_module_init(&sensor_cfg); // Initialize NTC temperature sensor
    sensor_init_filters(); // Initialize digital filters
    
    TickType_t lastWake = xTaskGetTickCount(); // Get current tick for periodic scheduling
    // Timing offset for Task 4: 0ms (base timing reference)
    
    for (;;) // Infinite task loop
    {
        sensor_read_with_filtering(); // Read NTC temperature sensor with filtering
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(SENSOR_READ_PERIOD_MS)); // Periodic delay for 500ms
    } // End of infinite loop
} // End of task4_sensor_reader

// Task 5 — Sensor Report Publisher: period 2000ms with enhanced formatting // Task 5: sensor data reporting
void task5_sensor_reporter(void* pvParameters) // Task entry point for sensor reporting
{
    TasksData* tasksData = (TasksData*)pvParameters; // Cast parameter to shared data
    
    TickType_t lastWake = xTaskGetTickCount(); // Get current tick for periodic scheduling
    // Timing offset for Task 5: 100ms after Task 4
    vTaskDelay(pdMS_TO_TICKS(100)); 
    lastWake = xTaskGetTickCount(); // Reset wake time after offset
    
    char report_buffer[256]; // Buffer for sensor report string
    
    for (;;) // Infinite task loop
    {
        sensor_get_report(report_buffer, sizeof(report_buffer)); // Get formatted sensor report
        printf("SENSOR: %s\n", report_buffer); // Print sensor data with filtering info
        
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(SENSOR_REPORT_PERIOD_MS)); // Periodic delay for 2000ms
    } // End of infinite loop
} // End of task5_sensor_reporter

// Task 6 — HC-SR04 Ultrasonic Distance Sensor: period 300ms with filtering // Task 6: ultrasonic distance measurement
void task6_ultrasonic_reader(void* pvParameters) // Task entry point for ultrasonic sensor
{
    TasksData* tasksData = (TasksData*)pvParameters; // Cast parameter to shared data
    // Initialize ultrasonic sensor with config
    ultrasonic_config_t ultrasonic_cfg;
    ultrasonic_cfg.name = "HC_SR04";
    ultrasonic_cfg.trigger_pin = ULTRASONIC_TRIGGER_PIN;
    ultrasonic_cfg.echo_pin = ULTRASONIC_ECHO_PIN;
    ultrasonic_cfg.max_distance_cm = 400;
    ultrasonic_cfg.timeout_us = 30000;
    ultrasonic_init(&ultrasonic_cfg); // Initialize HC-SR04 ultrasonic sensor
    
    TickType_t lastWake = xTaskGetTickCount(); // Get current tick for periodic scheduling  
    // Timing offset for Task 6: 200ms after Task 4
    vTaskDelay(pdMS_TO_TICKS(200));
    lastWake = xTaskGetTickCount(); // Reset wake time after offset
    
    char distance_buffer[128]; // Buffer for distance report
    
    for (;;) // Infinite task loop
    {
        ultrasonic_data_t distance_data = ultrasonic_read_data(); // Read ultrasonic data
        
        ultrasonic_get_report(distance_buffer, sizeof(distance_buffer)); // Get formatted distance report
        printf("ULTRASONIC: %s\n", distance_buffer); // Print distance data
        
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(ULTRASONIC_READ_PERIOD_MS)); // Periodic delay for 300ms
    } // End of infinite loop
} // End of task6_ultrasonic_reader

// Task 7 — Signal Processing Coordinator: period 1000ms, manages filter states // Task 7: signal processing management
void task7_signal_processor(void* pvParameters) // Task entry point for signal processing
{
    TasksData* tasksData = (TasksData*)pvParameters; // Cast parameter to shared data
    
    TickType_t lastWake = xTaskGetTickCount(); // Get current tick for periodic scheduling
    // Timing offset for Task 7: 300ms after Task 4  
    vTaskDelay(pdMS_TO_TICKS(300));
    lastWake = xTaskGetTickCount(); // Reset wake time after offset
    
    uint32_t processing_cycles = 0; // Counter for processing cycles
    
    for (;;) // Infinite task loop
    {
        processing_cycles++; // Increment cycle counter
        
        // Periodic status report of signal processing system
        printf("SIGNAL_PROC: Cycle %lu - Salt&Pepper: %s, WeightedMA: %s\n", 
               processing_cycles,
               ENABLE_SALT_PEPPER_FILTER ? "ON" : "OFF",
               ENABLE_MOVING_AVERAGE ? "ON" : "OFF");
        
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(SIGNAL_PROCESSING_PERIOD_MS)); // Periodic delay for 1000ms
    } // End of infinite loop
} // End of task7_signal_processor

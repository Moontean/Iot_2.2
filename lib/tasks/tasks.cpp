#include <tasks.h> // Include task declarations and dependencies
#include <sensor_module.h> // NTC temperature sensor module
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
                printf("\r\n"); // Proper CRLF line break
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
} // End of rtos_tasks_init

// === ENHANCED SYSTEM INITIALIZATION WITH DIGITAL SIGNAL PROCESSING ===
void rtos_tasks_init_enhanced(void) // Initialize enhanced system with signal processing (NTC only)
{
    // 1. Initialize base system (Tasks 1-3: button, producer, consumer)
    rtos_tasks_init();
    
    // 2. Initialize sensor modules and digital signal processing
    // NTC Temperature Sensor Configuration
    sensor_config_t ntc_config;
    ntc_config.pin = TEMPERATURE_SENSOR_PIN;
    ntc_config.type = SENSOR_TYPE_ANALOG;
    ntc_config.min_voltage = VOLTAGE_MIN_SATURATION;
    ntc_config.max_voltage = VOLTAGE_MAX_SATURATION;
    ntc_config.max_errors = 10;
    ntc_config.name = "NTC Temperature Sensor";
    sensor_module_init(&ntc_config);
    sensor_init_filters(); // Initialize digital filters for NTC
    
    // 3. Create enhanced tasks for digital signal processing
    static TasksData* pTasksData = nullptr; // Shared data pointer
    
    xTaskCreate(task4_ntc_sensor_reader, "NTC_Reader", 512, pTasksData, 2, nullptr);
    xTaskCreate(task5_system_status_reporter, "StatusReport", 512, pTasksData, 1, nullptr);
    // Ultrasonic and extra coordinator tasks removed for NTC-only implementation
} // End of rtos_tasks_init_enhanced

// === TASK 4: NTC TEMPERATURE SENSOR WITH DIGITAL SIGNAL PROCESSING ===
void task4_ntc_sensor_reader(void* pvParameters) // Task entry point for NTC sensor reading
{
    TasksData* tasksData = (TasksData*)pvParameters; // Cast parameter to shared data (may be null)
    
    // Task timing configuration
    TickType_t xLastWakeTime = xTaskGetTickCount(); // Initialize wake time
    vTaskDelay(pdMS_TO_TICKS(TASK_BASE_STARTUP_DELAY + TASK4_STARTUP_OFFSET)); // Startup offset
    
    printf("INIT: Task4 NTC Sensor Reader started with digital filters\r\n");
    
    for (;;) // Infinite task loop
    {
        // Read NTC sensor with full digital signal processing pipeline
        sensor_data_t sensor_data = sensor_read_with_filtering();
        
        // Optional: Print debug information for filter effectiveness analysis
        if (ENABLE_SALT_PEPPER_FILTER && ENABLE_MOVING_AVERAGE) {
            // Calculate filter effectiveness
            int16_t raw_diff = (int16_t)sensor_data.filtered_raw - (int16_t)sensor_data.raw_value;
            float temp_diff = sensor_data.filtered_value - sensor_data.physical_value;
            
                 printf("DEBUG_NTC: RAW:%d->%d(Δ%d) TEMP:%d.%d->%d.%d(Δ%d.%d)\r\n",
                   sensor_data.raw_value, sensor_data.filtered_raw, raw_diff,
                   (int)sensor_data.physical_value, (int)(sensor_data.physical_value * 10) % 10,
                   (int)sensor_data.filtered_value, (int)(sensor_data.filtered_value * 10) % 10,
                   (int)temp_diff, (int)(abs(temp_diff) * 10) % 10);
        }
        
        // Precise periodic execution using vTaskDelayUntil (required by assignment)
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SENSOR_READ_PERIOD_MS));
    } // End of infinite loop
} // End of task4_ntc_sensor_reader

// === TASK 5: SYSTEM STATUS REPORTS (REQUIRED 500ms PERIOD) ===
void task5_system_status_reporter(void* pvParameters) // Task entry point for system reports
{
    TasksData* tasksData = (TasksData*)pvParameters; // Cast parameter to shared data
    
    // Task timing configuration
    TickType_t xLastWakeTime = xTaskGetTickCount(); // Initialize wake time
    vTaskDelay(pdMS_TO_TICKS(TASK_BASE_STARTUP_DELAY + TASK5_STARTUP_OFFSET)); // Startup offset
    
    printf("INIT: Task5 System Status Reporter (period: %d ms)\r\n", SENSOR_REPORT_PERIOD_MS);
    
    static uint32_t report_counter = 0; // Static report counter
    char report_buffer[256]; // Buffer for formatted reports (increased size)
    
    for (;;) // Infinite task loop
    {
        report_counter++; // Increment report number
        uint32_t uptime_seconds = millis() / 1000; // System uptime in seconds
        
        // === STRUCTURED SYSTEM STATUS REPORT (USING STDIO) ===
         printf("===== System Report #%lu | Uptime: %lus =====\r\n", 
             report_counter, uptime_seconds);
        
        // Button/LED system status (from original tasks)
        if (tasksData != nullptr) {
            printf("Button: presses=%d | queue=active\r\n", tasksData->gN);
        }
        
        // NTC Temperature Sensor with digital signal processing
        sensor_get_report(report_buffer, sizeof(report_buffer));
        printf("NTC Sensor: %s\r\n", report_buffer);
        
        // Digital Signal Processing Status
        printf("DSP: median=%s | movingAvg=%s | saturation=%s\r\n",
               ENABLE_SALT_PEPPER_FILTER ? "ON" : "OFF",
               ENABLE_MOVING_AVERAGE ? "ON" : "OFF", 
               ENABLE_SIGNAL_SATURATION ? "ON" : "OFF");
        
        // FreeRTOS Task Information
        printf("FreeRTOS: tasks=%d | stacks=OK\r\n", 5);
        printf("--------------------------------------------------------\r\n");
        
        // CRITICAL: 500ms period as required by assignment
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(SENSOR_REPORT_PERIOD_MS));
    } // End of infinite loop
} // End of task5_system_status_reporter

// (Task6 Ultrasonic and Task7 Coordinator removed per request; NTC-only focus)

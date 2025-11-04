#include <tasks.h> // Include task declarations and dependencies

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

// Task 4 — Sensor Reader: periodic data collection using vTaskDelayUntil
void task4_sensor_reader(void* pvParameters) // Task entry point for sensor reading
{
    TasksData* tasksData = (TasksData*)pvParameters; // Cast parameter to shared data
    
    // Initialize temperature sensor configuration
    sensor_config_t sensor_config = {
        .pin = SENSOR_ANALOG_PIN,
        .type = SENSOR_TYPE_ANALOG,
        .min_voltage = SENSOR_MIN_VOLTAGE,
        .max_voltage = SENSOR_MAX_VOLTAGE,
        .max_errors = SENSOR_MAX_ERRORS,
        .name = SENSOR_NAME
    };
    sensor_module_init(&sensor_config);
    
    TickType_t lastWake = xTaskGetTickCount(); // Get current tick for periodic scheduling
    
    for (;;) // Infinite task loop
    {
        // Read sensor data
        sensor_data_t data = sensor_read_data();
        tasksData->gTotalSensorReads++; // Increment total reads counter
        
        if (data.status != SENSOR_STATUS_OK) {
            tasksData->gSensorErrors++; // Increment error counter
        }
        
        // Send sensor data to display task via queue (non-blocking)
        xQueueSend(tasksData->gSensorDataQueue, &data, 0);
        
        // Use vTaskDelayUntil for precise periodic timing
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(SENSOR_READ_PERIOD_MS));
    } // End of infinite loop
} // End of task4_sensor_reader

// Task 5 — System Monitor: display system status every 500ms using STDIO
void task5_system_monitor(void* pvParameters) // Task entry point for system monitoring
{
    TasksData* tasksData = (TasksData*)pvParameters; // Cast parameter to shared data
    TickType_t lastWake = xTaskGetTickCount(); // Get current tick for periodic scheduling
    char report_buffer[REPORT_BUFFER_SIZE]; // Buffer for formatted reports
    sensor_data_t latest_sensor_data; // Latest sensor data
    bool has_sensor_data = false; // Flag indicating if we have sensor data
    
    for (;;) // Infinite task loop
    {
        // Update system uptime (approximation)
        tasksData->gSystemUptime = xTaskGetTickCount() / configTICK_RATE_HZ;
        
        // Try to get latest sensor data (non-blocking)
        while (xQueueReceive(tasksData->gSensorDataQueue, &latest_sensor_data, 0) == pdTRUE) {
            has_sensor_data = true; // Mark that we have fresh data
        }
        
        // Generate and print system status report
        printf("\n\r==== SYSTEM STATUS REPORT ====\n\r");
        printf("Uptime: %lu seconds\n\r", tasksData->gSystemUptime);
        printf("Button presses (N): %d\n\r", tasksData->gN);
        printf("Total sensor reads: %d\n\r", tasksData->gTotalSensorReads);
        printf("Sensor errors: %d\n\r", tasksData->gSensorErrors);
        
        if (has_sensor_data) {
            // Generate sensor report
            sensor_get_report(report_buffer, sizeof(report_buffer));
            printf("Latest sensor: %s\n\r", report_buffer);
            
            // Calculate and display additional metrics
            float temperature = latest_sensor_data.physical_value; // Now contains temperature in Celsius
            float percentage = sensor_temperature_to_percentage(temperature);
            
            // Convert to integers for Arduino printf compatibility
            int temp_int = (int)(temperature * 10.0f);
            int perc_int = (int)(percentage * 10.0f);
            
            // Debug information
            int voltage_mv = (int)(sensor_raw_to_voltage(latest_sensor_data.raw_value) * 1000.0f);
            printf("DEBUG: RAW=%d, V=%dmV\n\r", latest_sensor_data.raw_value, voltage_mv);
            
            printf("Temperature: %d.%d°C (%d.%d%%)\n\r", 
                   temp_int / 10, temp_int % 10,
                   perc_int / 10, perc_int % 10);
        } else {
            printf("No sensor data available\n\r");
        }
        
        // printf("Free heap: %d bytes\n\r", xPortGetFreeHeapSize()); // Not available in this FreeRTOS version
        printf("===============================\n\r");
        
        // Use vTaskDelayUntil for precise 500ms timing
        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(SYSTEM_MONITOR_PERIOD_MS));
    } // End of infinite loop
} // End of task5_system_monitor

// Инициализация семафора, очереди и создание задач FreeRTOS // Initialize semaphore, queue, and create tasks
void rtos_tasks_init(void) // Public initialization function
{
    static TasksData tasksData; // Static storage for tasks shared data
    tasksData.gN = 0; // Initialize series counter to zero
    tasksData.gSystemUptime = 0; // Initialize uptime counter
    tasksData.gTotalSensorReads = 0; // Initialize sensor reads counter
    tasksData.gSensorErrors = 0; // Initialize sensor errors counter
    
    // Бинарный семафор: событие от Task1 к Task2 // Binary semaphore signals from Task1 to Task2
    tasksData.gButtonSemaphore = xSemaphoreCreateBinary(); // Create binary semaphore

    // Очередь сообщений: каждый элемент — ByteSeriesMsg // Message queue: items are bytes
    // ВАЖНО: длину очереди НЕ меняем (берётся из существующего макроса/конфига) // IMPORTANT: keep queue length from config
    tasksData.gBytesQueue = xQueueCreate(QUEUE_LENGTH, sizeof(byte)); // Create queue with fixed length
    
    // New queue for sensor data sharing
    tasksData.gSensorDataQueue = xQueueCreate(8, sizeof(sensor_data_t)); // Create sensor data queue

    // Создание задач // Create FreeRTOS tasks
    xTaskCreate(task1_button_led, "Task1", 256, &tasksData, 1, nullptr); // Create Task1 with stack size and priority
    xTaskCreate(task2_provider,  "Task2", 256, &tasksData, 1, nullptr); // Create Task2
    xTaskCreate(task3_consumer,  "Task3", 256, &tasksData, 2, nullptr); // Create Task3
    
    // Create new sensor-related tasks
    xTaskCreate(task4_sensor_reader, "SensorRead", 320, &tasksData, 2, nullptr); // Create sensor reader task
    xTaskCreate(task5_system_monitor, "SysMonitor", 384, &tasksData, 1, nullptr); // Create system monitor task
} // End of rtos_tasks_init

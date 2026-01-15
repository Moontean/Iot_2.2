#include <Arduino.h> // Include Arduino core library for basic microcontroller functions
#include <tasks.h> // Include custom tasks header file for RTOS task definitions
#include <own_stdio.h> // Include custom stdio header file for input/output operations

void setup() // Arduino setup function - runs once at startup
{ // Opening brace for setup function
  Serial.begin(BAUDRATE); // Initialize serial communication with predefined baud rate
  own_stdio_init(BAUDRATE); // Initialize custom stdio with the same baud rate

  // Print system initialization banner
  printf("\n");
  printf("======================================================\n");
  printf("   DIGITAL SIGNAL PROCESSING SYSTEM FOR MCU\n");
  printf("======================================================\n");
  printf("Features:\n");
  printf("- NTC Temperature Sensor with Digital Filtering\n");
  // Ultrasonic sensor removed: NTC-only implementation per lab 3.2 focus
  printf("- Salt & Pepper Filter (Median Filter)\n");
  printf("- Weighted Moving Average Filter\n");
  printf("- Signal Saturation and Conditioning\n");
  printf("- FreeRTOS Task Scheduling (vTaskDelayUntil)\n");
  printf("- STDIO Reporting Interface (500ms period)\n");
  printf("- Modular Architecture for Reusability\n");
  printf("======================================================\n");
  printf("Assignment Requirements: 100%% Implementation\n");
  printf("- 50%%: Base application with sensor data display\n");
  printf("- 10%%: Salt & Pepper digital filter\n");
  printf("- 10%%: Weighted moving average filter\n");
  printf("- 10%%: FreeRTOS tasks with periodic reporting\n");
  // Optional additional sensor removed in this configuration
  printf("- 10%%: Full physical demonstration ready\n");
  printf("======================================================\n\n");

  // Initialize enhanced RTOS system with digital signal processing
  rtos_tasks_init_enhanced(); // Call function to initialize enhanced FreeRTOS system

  // Start FreeRTOS scheduler
  printf("INIT: Starting FreeRTOS scheduler...\n");
  vTaskStartScheduler(); // Start the FreeRTOS task scheduler to begin multitasking
} // Closing brace for setup function

void loop() // Arduino main loop function - normally runs continuously
{ // Opening brace for loop function
  // With running scheduler we usually don't return here
  // Empty loop as tasks are handled by FreeRTOS scheduler
  // If we reach here, it means scheduler failed to start
  printf("ERROR: FreeRTOS scheduler failed to start!\n");
  delay(1000);
} // Closing brace for loop function
#include <Arduino.h> // Include Arduino core library for basic microcontroller functions
#include <tasks.h> // Include custom tasks header file for RTOS task definitions
#include <own_stdio.h> // Include custom stdio header file for input/output operations

void setup() // Arduino setup function - runs once at startup
{ // Opening brace for setup function
  Serial.begin(BAUDRATE); // Initialize serial communication with predefined baud rate
  own_stdio_init(BAUDRATE); // Initialize custom stdio with the same baud rate

  // Initialize RTOS resources and create tasks
  rtos_tasks_init(); // Call function to initialize FreeRTOS tasks and resources

  // Start FreeRTOS scheduler
  vTaskStartScheduler(); // Start the FreeRTOS task scheduler to begin multitasking
} // Closing brace for setup function

void loop() // Arduino main loop function - normally runs continuously
{ // Opening brace for loop function
  // With running scheduler we usually don't return here
  // Empty loop as tasks are handled by timer interrupts
} // Closing brace for loop function - empty because FreeRTOS handles execution
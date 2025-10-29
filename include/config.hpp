#ifndef PIN_CONFIG_H // Header guard to prevent multiple inclusions of this file
#define PIN_CONFIG_H // Define the header guard macro

#include <Arduino.h> // Include Arduino core library for basic functions and definitions

//LED pins
#define LED_PIN 13 // Define pin 13 for the main LED
#define GREEN_LED_PIN 12 // Define pin 12 for the green LED
#define BLUE_LED_PIN 10 // Define pin 10 for the blue LED

//Button pins
#define BUTTON_PIN 2 // Define pin 2 for the main button
#define ON_OFF_BUTTON_PIN 6 // Define pin 6 for the on/off button
#define UP_BUTTON_PIN 3 // Define pin 3 for the up button
#define DOWN_BUTTON_PIN 1 // Define pin 1 for the down button

//Keypad pins (4x4)
#define KEYPAD_ROWS 4 // Define number of rows in the keypad (4 rows)
#define KEYPAD_COLS 4 // Define number of columns in the keypad (4 columns)

enum KEYPAD_PINS // Enumeration to define keypad pin assignments
{
    KEYPAD_ROW_1_PIN = 39, // Pin 39 assigned to keypad row 1
    KEYPAD_ROW_2_PIN = 41, // Pin 41 assigned to keypad row 2
    KEYPAD_ROW_3_PIN = 43, // Pin 43 assigned to keypad row 3
    KEYPAD_ROW_4_PIN = 45, // Pin 45 assigned to keypad row 4
    KEYPAD_COL_1_PIN = 47, // Pin 47 assigned to keypad column 1
    KEYPAD_COL_2_PIN = 49, // Pin 49 assigned to keypad column 2
    KEYPAD_COL_3_PIN = 51, // Pin 51 assigned to keypad column 3
    KEYPAD_COL_4_PIN = 53, // Pin 53 assigned to keypad column 4
}; // End of keypad pins enumeration

//Password configuration
#define PASSWORD_LENGTH 4 // Define password length as 4 characters
#define PASSWORD "1234" // Define default password as "1234"

//Led timeout configuration
#define LED_GREEN_TIMEOUT 3000 // Define green LED timeout as 3000 milliseconds (3 seconds)
#define LED_RED_TIMEOUT 3000 // Define red LED timeout as 3000 milliseconds (3 seconds)

//Serial configuration
#define BAUDRATE 115200 // Define serial communication baud rate as 115200 bits per second

#define INPUT_BUFFER_SIZE 10 // Define input buffer size as 10 bytes

//lcd configuration
#define LCD_ADDRESS 0x27 // Define I2C address for LCD display as 0x27
#define LCD_COLUMNS 16 // Define LCD display width as 16 columns
#define LCD_ROWS 2 // Define LCD display height as 2 rows

#define DEBOUNCE_TIME_MS 200 // Define button debounce time as 200 milliseconds

#define DEFAULT_FREQUENCY_MS 500 // Define default frequency as 500 milliseconds
#define FIRST_TASK_OFFSET_MS 1 // Define first task offset as 1 millisecond
#define FIRST_TASK_RECURRENCE_MS 10 // Define first task recurrence interval as 10 milliseconds

#define SECOND_TASK_OFFSET_MS 5 // Define second task offset as 5 milliseconds
#define SECOND_TASK_RECURRENCE_MS 7 // Define second task recurrence interval as 7 milliseconds

#define LED_ON_TIME_MS 1000 // Define LED on time as 1000 milliseconds (1 second)

#define SECOND_LED_ON_TIME_MS 300 // Define second LED on time as 300 milliseconds
#define SECOND_LED_OFF_TIME_MS 500 // Define second LED off time as 500 milliseconds

// FreeRTOS task and queue parameters
#define TASK1_PERIOD_MS          10 // Define task 1 period as 10 milliseconds
#define TASK2_SEND_INTERVAL_MS   50 // Define task 2 send interval as 50 milliseconds
#define TASK2_LED_ON_MS          300 // Define task 2 LED on time as 300 milliseconds
#define TASK2_LED_OFF_MS         500 // Define task 2 LED off time as 500 milliseconds
#define TASK3_PERIOD_MS          200 // Define task 3 period as 200 milliseconds

// Provider/consumer queue parameters (bytes)
#define QUEUE_LENGTH             32 // Define queue length as 32 items
#define QUEUE_ITEM_SIZE          1 // Define queue item size as 1 byte
#endif // End of header guard


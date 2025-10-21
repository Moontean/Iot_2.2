#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

#include <Arduino.h>

//LED pins
#define LED_PIN 13
#define GREEN_LED_PIN 12
#define BLUE_LED_PIN 10

//Button pins
#define BUTTON_PIN 2
#define ON_OFF_BUTTON_PIN 6
#define UP_BUTTON_PIN 3
#define DOWN_BUTTON_PIN 1

//Keypad pins (4x4)
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

enum KEYPAD_PINS
{
    KEYPAD_ROW_1_PIN = 39,
    KEYPAD_ROW_2_PIN = 41,
    KEYPAD_ROW_3_PIN = 43,
    KEYPAD_ROW_4_PIN = 45,
    KEYPAD_COL_1_PIN = 47,
    KEYPAD_COL_2_PIN = 49,
    KEYPAD_COL_3_PIN = 51,
    KEYPAD_COL_4_PIN = 53,
};

//Password configuration
#define PASSWORD_LENGTH 4
#define PASSWORD "1234"

//Led timeout configuration
#define LED_GREEN_TIMEOUT 3000
#define LED_RED_TIMEOUT 3000

//Serial configuration
#define BAUDRATE 115200

#define INPUT_BUFFER_SIZE 10

//lcd configuration
#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

#define DEBOUNCE_TIME_MS 200

#define DEFAULT_FREQUENCY_MS 500
#define FIRST_TASK_OFFSET_MS 1
#define FIRST_TASK_RECURRENCE_MS 10

#define SECOND_TASK_OFFSET_MS 5
#define SECOND_TASK_RECURRENCE_MS 7

#define LED_ON_TIME_MS 1000

#define SECOND_LED_ON_TIME_MS 300
#define SECOND_LED_OFF_TIME_MS 500

// Параметры FreeRTOS-задач и очереди
#define TASK1_PERIOD_MS          10
#define TASK2_SEND_INTERVAL_MS   50
#define TASK2_LED_ON_MS          300
#define TASK2_LED_OFF_MS         500
#define TASK3_PERIOD_MS          200

// Очередь provider/consumer (байты)
#define QUEUE_LENGTH             32
#define QUEUE_ITEM_SIZE          1
#endif


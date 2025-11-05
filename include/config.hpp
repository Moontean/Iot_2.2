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

//Sensor pins
#define TEMPERATURE_SENSOR_PIN A0 // Analog pin A0 for NTC temperature sensor

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

// === УЛЬТРАЗВУКОВОЙ ДАТЧИК HC-SR04 ===
#define ULTRASONIC_TRIGGER_PIN     7     // Пин триггера HC-SR04
#define ULTRASONIC_ECHO_PIN        8     // Пин эхо HC-SR04  
#define ULTRASONIC_MIN_DISTANCE_CM 2.0f  // Минимальное расстояние (см)
#define ULTRASONIC_MAX_DISTANCE_CM 400.0f // Максимальное расстояние (см)
#define ULTRASONIC_TIMEOUT_US      30000  // Таймаут измерения (мкс)
#define ULTRASONIC_MAX_ERRORS      5      // Максимальное количество ошибок подряд
#define ULTRASONIC_NAME            "HC-SR04 Ultrasonic" // Имя датчика для отчетов

// === ЦИФРОВАЯ ОБРАБОТКА СИГНАЛОВ ===
#define ENABLE_SALT_PEPPER_FILTER  1     // Включить фильтр "соль и перец" (медианный)
#define ENABLE_MOVING_AVERAGE      1     // Включить взвешенный скользящий фильтр
#define ENABLE_SATURATION          1     // Включить насыщение (ограничение диапазона)

// === ПАРАМЕТРЫ NTC ДАТЧИКА ТЕМПЕРАТУРЫ ===
#define TEMP_PULLUP_RESISTOR    10000.0f    // Подтягивающий резистор 10кОм
#define TEMP_SENSOR_R25         10000.0f    // Сопротивление NTC при 25°C (10кОм)
#define TEMP_SENSOR_T0          298.15f     // Температура 25°C в Кельвинах (298.15К)
#define TEMP_SENSOR_BETA        3950.0f     // Бета-коэффициент NTC (3950К)
#define TEMP_MIN_CELSIUS        -40.0f      // Минимальная температура диапазона
#define TEMP_MAX_CELSIUS        125.0f      // Максимальная температура диапазона

// === ПЕРИОДЫ ЗАДАЧ FreeRTOS ===
#define SENSOR_READ_PERIOD_MS       500     // Период чтения NTC датчика (500мс)
#define SENSOR_REPORT_PERIOD_MS     2000    // Период отчетов датчика (2сек)
#define ULTRASONIC_READ_PERIOD_MS   300     // Период чтения ультразвукового датчика
#define SIGNAL_PROCESSING_PERIOD_MS 1000    // Период обработки сигналов (1сек)

// === ЗАДЕРЖКИ И СМЕЩЕНИЯ ЗАДАЧ ===
#define TASK_STARTUP_DELAY_MS       50   // Базовая задержка запуска задач
#define ULTRASONIC_STARTUP_DELAY    100  // Смещение для ультразвукового датчика
#define PROCESSING_STARTUP_DELAY    75   // Смещение для задачи обработки сигналов
#define MONITOR_STARTUP_DELAY       150  // Смещение для задачи мониторинга

// === ОГРАНИЧЕНИЯ НАСЫЩЕНИЯ ===
#define SENSOR_MIN_TEMP_SATURATION  -45.0f // Минимальная температура для насыщения
#define SENSOR_MAX_TEMP_SATURATION  130.0f // Максимальная температура для насыщения

#endif // End of header guard


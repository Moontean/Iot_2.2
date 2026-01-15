#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

#include <Arduino.h>
#include <stdint.h>
#include <config.hpp>

// Статусы ультразвукового датчика HC-SR04
typedef enum {
    ULTRASONIC_STATUS_OK = 0,           // Измерение успешно
    ULTRASONIC_STATUS_ERROR,            // Общая ошибка
    ULTRASONIC_STATUS_OUT_OF_RANGE,     // Значение вне допустимого диапазона
    ULTRASONIC_STATUS_TIMEOUT,          // Таймаут при ожидании эхо-сигнала
    ULTRASONIC_STATUS_NOT_CONNECTED     // Датчик не подключен
} ultrasonic_status_t;

// Структура данных ультразвукового датчика
typedef struct {
    uint32_t pulse_duration_us;     // Длительность импульса в микросекундах
    float distance_cm;              // Расстояние в сантиметрах
    float filtered_distance_cm;     // Отфильтрованное расстояние в сантиметрах
    float distance_percentage;      // Процент от максимального расстояния (0-100%)
    ultrasonic_status_t status;     // Статус последнего измерения
    uint32_t timestamp_ms;          // Время измерения в миллисекундах
    uint16_t error_count;           // Счетчик последовательных ошибок
} ultrasonic_data_t;

// Конфигурация ультразвукового датчика
typedef struct {
    uint8_t trigger_pin;            // Пин триггера
    uint8_t echo_pin;               // Пин эхо
    float min_distance_cm;          // Минимальное измеряемое расстояние
    float max_distance_cm;          // Максимальное измеряемое расстояние
    uint32_t timeout_us;            // Таймаут ожидания эхо в микросекундах
    uint16_t max_errors;            // Максимальное количество ошибок подряд
    const char* name;               // Имя датчика для отчетов
} ultrasonic_config_t;

// Функции инициализации и работы с ультразвуковым датчиком
void ultrasonic_init(const ultrasonic_config_t* config);
ultrasonic_data_t ultrasonic_read_data(void);
ultrasonic_status_t ultrasonic_get_status(void);
const ultrasonic_config_t* ultrasonic_get_config(void);

// Функции преобразования данных
float ultrasonic_pulse_to_distance(uint32_t pulse_duration_us);
float ultrasonic_distance_to_percentage(float distance_cm);
bool ultrasonic_is_distance_valid(float distance_cm);

// Функции для генерации отчетов
void ultrasonic_get_report(char* buffer, size_t buffer_size);
void ultrasonic_reset_errors(void);

// Внутренние функции (могут быть использованы для отладки)
void ultrasonic_trigger_pulse(void);
uint32_t ultrasonic_measure_echo(void);

#endif // ULTRASONIC_SENSOR_H
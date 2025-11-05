#ifndef SIGNAL_PROCESSING_H
#define SIGNAL_PROCESSING_H

#include <Arduino.h>
#include <stdint.h>

// Размеры буферов для фильтров
#define MEDIAN_FILTER_SIZE 5      // Размер буфера для медианного фильтра (соль и перец)
#define MOVING_AVERAGE_SIZE 8     // Размер буфера для взвешенного скользящего фильтра

// Структура для хранения данных фильтров
typedef struct {
    uint16_t median_buffer[MEDIAN_FILTER_SIZE];  // Буфер для медианного фильтра
    uint8_t median_index;                        // Индекс для кольцевого буфера медианного фильтра
    
    float average_buffer[MOVING_AVERAGE_SIZE];   // Буфер для скользящего среднего
    float weights[MOVING_AVERAGE_SIZE];          // Веса для взвешенного усреднения
    uint8_t average_index;                       // Индекс для кольцевого буфера скользящего среднего
    
    uint16_t sample_count;                       // Счетчик обработанных образцов
} FilterData_t;

// Функции инициализации и работы с фильтрами
void filter_init(FilterData_t* filter);

// Фильтр "соль и перец" (медианный фильтр) для устранения импульсных шумов
uint16_t salt_pepper_filter(FilterData_t* filter, uint16_t raw_value);

// Взвешенный скользящий фильтр усреднения для сглаживания сигнала
float weighted_moving_average(FilterData_t* filter, float input);

// Функция насыщения (saturation) для ограничения значений в допустимом диапазоне
float apply_saturation(float value, float min_val, float max_val);

// Вспомогательная функция сортировки для медианного фильтра
void bubble_sort(uint16_t* arr, uint8_t size);

// Функция для получения медианы из массива
uint16_t get_median(uint16_t* sorted_array, uint8_t size);

#endif // SIGNAL_PROCESSING_H
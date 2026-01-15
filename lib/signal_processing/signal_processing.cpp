#include "signal_processing.h"
#include <math.h>

// Инициализация структуры фильтров
void filter_init(FilterData_t* filter) {
    if (filter == nullptr) return;
    
    // Инициализация медианного фильтра средним значением ADC
    for (uint8_t i = 0; i < MEDIAN_FILTER_SIZE; i++) {
        filter->median_buffer[i] = 512; // Среднее значение 10-битного ADC (0-1023)
    }
    filter->median_index = 0;
    
    // Инициализация взвешенного фильтра
    // Веса убывающие: новые данные важнее старых
    float weight_sum = 0.0f;
    for (uint8_t i = 0; i < MOVING_AVERAGE_SIZE; i++) {
        // Вес уменьшается экспоненциально для старых значений
        filter->weights[i] = (float)(MOVING_AVERAGE_SIZE - i) / (float)MOVING_AVERAGE_SIZE;
        weight_sum += filter->weights[i];
        filter->average_buffer[i] = 25.0f; // Инициализация комнатной температурой
    }
    
    // Нормализация весов чтобы их сумма была равна 1.0
    for (uint8_t i = 0; i < MOVING_AVERAGE_SIZE; i++) {
        filter->weights[i] /= weight_sum;
    }
    
    filter->average_index = 0;
    filter->sample_count = 0;
}

// Реализация фильтра "соль и перец" (медианный фильтр)
uint16_t salt_pepper_filter(FilterData_t* filter, uint16_t raw_value) {
    if (filter == nullptr) return raw_value;
    
    // Добавляем новое значение в кольцевой буфер
    filter->median_buffer[filter->median_index] = raw_value;
    filter->median_index = (filter->median_index + 1) % MEDIAN_FILTER_SIZE;
    
    // Создаем копию буфера для сортировки (не изменяем оригинальный)
    uint16_t sorted_buffer[MEDIAN_FILTER_SIZE];
    for (uint8_t i = 0; i < MEDIAN_FILTER_SIZE; i++) {
        sorted_buffer[i] = filter->median_buffer[i];
    }
    
    // Сортируем копию и возвращаем медиану
    bubble_sort(sorted_buffer, MEDIAN_FILTER_SIZE);
    return get_median(sorted_buffer, MEDIAN_FILTER_SIZE);
}

// Реализация взвешенного скользящего фильтра усреднения
float weighted_moving_average(FilterData_t* filter, float input) {
    if (filter == nullptr) return input;
    
    // Добавляем новое значение в кольцевой буфер
    filter->average_buffer[filter->average_index] = input;
    filter->average_index = (filter->average_index + 1) % MOVING_AVERAGE_SIZE;
    
    // Увеличиваем счетчик образцов (до максимума)
    if (filter->sample_count < MOVING_AVERAGE_SIZE) {
        filter->sample_count++;
    }
    
    // Вычисляем взвешенное среднее
    float weighted_sum = 0.0f;
    uint8_t samples_to_use = filter->sample_count; // Используем только доступные образцы
    
    for (uint8_t i = 0; i < samples_to_use; i++) {
        // Получаем индекс элемента, начиная с самого нового
        uint8_t buffer_idx = (filter->average_index - 1 - i + MOVING_AVERAGE_SIZE) % MOVING_AVERAGE_SIZE;
        weighted_sum += filter->average_buffer[buffer_idx] * filter->weights[i];
    }
    
    return weighted_sum;
}

// Функция насыщения для ограничения значений в диапазоне
float apply_saturation(float value, float min_val, float max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

// Простая сортировка пузырьком для небольших массивов
void bubble_sort(uint16_t* arr, uint8_t size) {
    if (arr == nullptr || size <= 1) return;
    
    for (uint8_t i = 0; i < size - 1; i++) {
        for (uint8_t j = 0; j < size - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Обмен элементов
                uint16_t temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// Получение медианы из отсортированного массива
uint16_t get_median(uint16_t* sorted_array, uint8_t size) {
    if (sorted_array == nullptr || size == 0) return 0;
    
    // Для нечетного размера возвращаем средний элемент
    // Для четного размера возвращаем среднее арифметическое двух средних элементов
    if (size % 2 == 1) {
        return sorted_array[size / 2];
    } else {
        return (sorted_array[size / 2 - 1] + sorted_array[size / 2]) / 2;
    }
}
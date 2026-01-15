#include "ultrasonic_sensor.h"
#include "signal_processing.h"

// Статические переменные для модуля ультразвукового датчика
static ultrasonic_config_t g_ultrasonic_config;
static ultrasonic_data_t g_ultrasonic_data;
static bool g_module_initialized = false;

// Инициализация ультразвукового датчика с заданной конфигурацией
void ultrasonic_init(const ultrasonic_config_t* config) {
    if (config == nullptr) return;
    
    // Копируем конфигурацию
    g_ultrasonic_config = *config;
    
    // Инициализируем пины
    pinMode(g_ultrasonic_config.trigger_pin, OUTPUT);
    pinMode(g_ultrasonic_config.echo_pin, INPUT);
    digitalWrite(g_ultrasonic_config.trigger_pin, LOW);
    
    // Инициализируем структуру данных
    g_ultrasonic_data.pulse_duration_us = 0;
    g_ultrasonic_data.distance_cm = 0.0f;
    g_ultrasonic_data.filtered_distance_cm = 0.0f;
    g_ultrasonic_data.distance_percentage = 0.0f;
    g_ultrasonic_data.status = ULTRASONIC_STATUS_OK;
    g_ultrasonic_data.timestamp_ms = 0;
    g_ultrasonic_data.error_count = 0;
    
    g_module_initialized = true;
}

// Чтение данных с ультразвукового датчика
ultrasonic_data_t ultrasonic_read_data(void) {
    if (!g_module_initialized) {
        g_ultrasonic_data.status = ULTRASONIC_STATUS_NOT_CONNECTED;
        return g_ultrasonic_data;
    }
    
    // Получаем текущее время
    g_ultrasonic_data.timestamp_ms = millis();
    
    // Генерируем триггерный импульс
    ultrasonic_trigger_pulse();
    
    // Измеряем длительность эхо-импульса
    g_ultrasonic_data.pulse_duration_us = ultrasonic_measure_echo();
    
    if (g_ultrasonic_data.pulse_duration_us == 0) {
        // Таймаут - не получили эхо-сигнал
        g_ultrasonic_data.status = ULTRASONIC_STATUS_TIMEOUT;
        g_ultrasonic_data.distance_cm = -1.0f;
        g_ultrasonic_data.filtered_distance_cm = -1.0f;
        g_ultrasonic_data.distance_percentage = 0.0f;
        g_ultrasonic_data.error_count++;
        return g_ultrasonic_data;
    }
    
    // Преобразуем длительность импульса в расстояние
    g_ultrasonic_data.distance_cm = ultrasonic_pulse_to_distance(g_ultrasonic_data.pulse_duration_us);
    
    // Применяем насыщение к измеренному расстоянию
    g_ultrasonic_data.distance_cm = apply_saturation(g_ultrasonic_data.distance_cm,
                                                     g_ultrasonic_config.min_distance_cm,
                                                     g_ultrasonic_config.max_distance_cm);
    
    // Проверяем валидность измерения
    if (ultrasonic_is_distance_valid(g_ultrasonic_data.distance_cm)) {
        g_ultrasonic_data.status = ULTRASONIC_STATUS_OK;
        g_ultrasonic_data.distance_percentage = ultrasonic_distance_to_percentage(g_ultrasonic_data.distance_cm);
        g_ultrasonic_data.error_count = 0; // Сбрасываем счетчик ошибок при успешном измерении
    } else {
        g_ultrasonic_data.status = ULTRASONIC_STATUS_OUT_OF_RANGE;
        g_ultrasonic_data.distance_percentage = 0.0f;
        g_ultrasonic_data.error_count++;
        
        // Если слишком много ошибок подряд, помечаем как общую ошибку
        if (g_ultrasonic_data.error_count >= g_ultrasonic_config.max_errors) {
            g_ultrasonic_data.status = ULTRASONIC_STATUS_ERROR;
        }
    }
    
    return g_ultrasonic_data;
}

// Получение текущего статуса датчика
ultrasonic_status_t ultrasonic_get_status(void) {
    return g_ultrasonic_data.status;
}

// Получение конфигурации датчика
const ultrasonic_config_t* ultrasonic_get_config(void) {
    return g_module_initialized ? &g_ultrasonic_config : nullptr;
}

// Преобразование длительности импульса в расстояние
float ultrasonic_pulse_to_distance(uint32_t pulse_duration_us) {
    // Формула: distance = (pulse_duration * sound_speed) / 2
    // Скорость звука в воздухе при комнатной температуре: ~343 м/с = 0.0343 см/мкс
    // Деление на 2 потому что звук проходит путь туда и обратно
    const float SOUND_SPEED_CM_PER_US = 0.0343f;
    return (float)pulse_duration_us * SOUND_SPEED_CM_PER_US / 2.0f;
}

// Преобразование расстояния в процентное значение (0-100%)
float ultrasonic_distance_to_percentage(float distance_cm) {
    const float min_dist = g_ultrasonic_config.min_distance_cm;
    const float max_dist = g_ultrasonic_config.max_distance_cm;
    
    if (max_dist <= min_dist) return 0.0f;
    
    // Ограничиваем расстояние допустимым диапазоном
    if (distance_cm < min_dist) distance_cm = min_dist;
    if (distance_cm > max_dist) distance_cm = max_dist;
    
    // Вычисляем процентное значение
    return ((distance_cm - min_dist) / (max_dist - min_dist)) * 100.0f;
}

// Проверка валидности расстояния
bool ultrasonic_is_distance_valid(float distance_cm) {
    return (distance_cm >= g_ultrasonic_config.min_distance_cm && 
            distance_cm <= g_ultrasonic_config.max_distance_cm);
}

// Генерация отчета о состоянии датчика
void ultrasonic_get_report(char* buffer, size_t buffer_size) {
    if (buffer == nullptr || buffer_size == 0) return;
    
    if (!g_module_initialized) {
        snprintf(buffer, buffer_size, "ULTRASONIC: Not initialized");
        return;
    }
    
    const char* status_str;
    switch (g_ultrasonic_data.status) {
        case ULTRASONIC_STATUS_OK:
            status_str = "OK";
            break;
        case ULTRASONIC_STATUS_ERROR:
            status_str = "ERROR";
            break;
        case ULTRASONIC_STATUS_OUT_OF_RANGE:
            status_str = "OUT_OF_RANGE";
            break;
        case ULTRASONIC_STATUS_TIMEOUT:
            status_str = "TIMEOUT";
            break;
        case ULTRASONIC_STATUS_NOT_CONNECTED:
            status_str = "NOT_CONNECTED";
            break;
        default:
            status_str = "UNKNOWN";
            break;
    }
    
    // Преобразуем в целые числа для совместимости с Arduino printf
    int dist_int = (int)(g_ultrasonic_data.distance_cm * 10.0f);
    int perc_int = (int)(g_ultrasonic_data.distance_percentage * 10.0f);
    
    snprintf(buffer, buffer_size,
        "%s: %d.%dcm (%d.%d%%) [PULSE:%lu us] [%s] [ERR:%d] [TIME:%lu]",
        g_ultrasonic_config.name,
        dist_int / 10, dist_int % 10,        // Расстояние с одним знаком после запятой
        perc_int / 10, perc_int % 10,        // Проценты с одним знаком после запятой
        g_ultrasonic_data.pulse_duration_us,
        status_str,
        g_ultrasonic_data.error_count,
        g_ultrasonic_data.timestamp_ms
    );
}

// Сброс счетчика ошибок
void ultrasonic_reset_errors(void) {
    g_ultrasonic_data.error_count = 0;
    if (g_ultrasonic_data.status == ULTRASONIC_STATUS_ERROR) {
        g_ultrasonic_data.status = ULTRASONIC_STATUS_OK;
    }
}

// Генерация триггерного импульса
void ultrasonic_trigger_pulse(void) {
    // Генерируем короткий импульс на триггерном пине
    digitalWrite(g_ultrasonic_config.trigger_pin, LOW);
    delayMicroseconds(2);  // Убеждаемся что пин в LOW состоянии
    digitalWrite(g_ultrasonic_config.trigger_pin, HIGH);
    delayMicroseconds(10); // Генерируем 10 мкс импульс
    digitalWrite(g_ultrasonic_config.trigger_pin, LOW);
}

// Измерение длительности эхо-импульса
uint32_t ultrasonic_measure_echo(void) {
    // Используем pulseIn для измерения длительности HIGH импульса на эхо-пине
    // Функция возвращает 0 в случае таймаута
    return pulseIn(g_ultrasonic_config.echo_pin, HIGH, g_ultrasonic_config.timeout_us);
}
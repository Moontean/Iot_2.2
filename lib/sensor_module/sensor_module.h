#ifndef SENSOR_MODULE_H // Header guard for sensor module
#define SENSOR_MODULE_H // Define header guard macro

#include <Arduino.h> // Arduino core library
#include <config.hpp> // Project configuration

// Sensor types enumeration
typedef enum {
    SENSOR_TYPE_ANALOG = 0,    // Analog sensor (potentiometer, photoresistor, etc.)
    SENSOR_TYPE_DIGITAL = 1    // Digital sensor (temperature, humidity, etc.)
} sensor_type_t;

// Sensor status enumeration
typedef enum {
    SENSOR_STATUS_OK = 0,        // Sensor working normally
    SENSOR_STATUS_ERROR = 1,     // Sensor read error
    SENSOR_STATUS_OUT_OF_RANGE = 2, // Value out of expected range
    SENSOR_STATUS_NOT_CONNECTED = 3  // Sensor not connected/responding
} sensor_status_t;

// Sensor data structure
typedef struct {
    uint16_t raw_value;        // Raw ADC value (0-1023 for analog)
    uint16_t filtered_raw;     // Filtered RAW ADC value after salt&pepper filter
    float physical_value;      // Converted physical value (temperature in Celsius)
    float filtered_value;      // Filtered physical value after moving average
    sensor_status_t status;    // Current sensor status
    uint32_t timestamp_ms;     // When the reading was taken
    uint16_t error_count;      // Number of consecutive errors
} sensor_data_t;

// Sensor configuration structure
typedef struct {
    uint8_t pin;               // Analog pin number (A0-A15)
    sensor_type_t type;        // Sensor type
    float min_voltage;         // Minimum expected voltage (for range checking)
    float max_voltage;         // Maximum expected voltage (for range checking)
    uint16_t max_errors;       // Maximum allowed consecutive errors
    const char* name;          // Sensor name for reporting
} sensor_config_t;

// Function prototypes

// Initialize sensor module
void sensor_module_init(const sensor_config_t* config);

// Read sensor data (thread-safe)
sensor_data_t sensor_read_data(void);

// Get sensor status
sensor_status_t sensor_get_status(void);

// Get sensor configuration
const sensor_config_t* sensor_get_config(void);

// Convert raw ADC value to voltage
float sensor_raw_to_voltage(uint16_t raw_value);

// Convert voltage to percentage (0-100%)
float sensor_voltage_to_percentage(float voltage);

// Temperature sensor specific functions
float sensor_voltage_to_temperature(float voltage);

// Convert temperature to normalized percentage for display
float sensor_temperature_to_percentage(float temperature);

// Check if sensor value is in valid range
bool sensor_is_value_valid(float voltage);

// Check if temperature is in valid range
bool sensor_is_temperature_valid(float temperature);

// Enhanced functions with digital signal processing
sensor_data_t sensor_read_with_filtering(void);
void sensor_init_filters(void);

// Reset error counter
void sensor_reset_errors(void);

// Get formatted sensor report string
void sensor_get_report(char* buffer, size_t buffer_size);

#endif // SENSOR_MODULE_H
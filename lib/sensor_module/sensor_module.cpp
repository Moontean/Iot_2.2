#include "sensor_module.h"

// Static variables for sensor module
static sensor_config_t g_sensor_config;
static sensor_data_t g_sensor_data;
static bool g_module_initialized = false;

// Initialize sensor module with given configuration
void sensor_module_init(const sensor_config_t* config) 
{
    if (config == nullptr) return;
    
    // Copy configuration
    g_sensor_config = *config;
    
    // Initialize sensor data
    g_sensor_data.raw_value = 0;
    g_sensor_data.physical_value = 0.0f;
    g_sensor_data.status = SENSOR_STATUS_OK;
    g_sensor_data.timestamp_ms = 0;
    g_sensor_data.error_count = 0;
    
    // Set ADC reference to default (5V on Mega 2560)
    analogReference(DEFAULT);
    
    g_module_initialized = true;
}

// Read sensor data from configured pin
sensor_data_t sensor_read_data(void) 
{
    if (!g_module_initialized) {
        g_sensor_data.status = SENSOR_STATUS_NOT_CONNECTED;
        return g_sensor_data;
    }
    
    // Get current timestamp
    g_sensor_data.timestamp_ms = millis();
    
    if (g_sensor_config.type == SENSOR_TYPE_ANALOG) {
        // Read analog value
        uint16_t raw = analogRead(g_sensor_config.pin);
        g_sensor_data.raw_value = raw;
        
        // Convert to voltage
        float voltage = sensor_raw_to_voltage(raw);
        
        // Convert voltage to temperature (Celsius) for temperature sensor
        float temperature = sensor_voltage_to_temperature(voltage);
        g_sensor_data.physical_value = temperature; // Store temperature instead of voltage
        
        // Check if temperature is valid
        if (sensor_is_temperature_valid(temperature)) {
            g_sensor_data.status = SENSOR_STATUS_OK;
            g_sensor_data.error_count = 0; // Reset error count on successful read
        } else {
            g_sensor_data.status = SENSOR_STATUS_OUT_OF_RANGE;
            g_sensor_data.error_count++;
            
            // If too many errors, mark as error
            if (g_sensor_data.error_count >= g_sensor_config.max_errors) {
                g_sensor_data.status = SENSOR_STATUS_ERROR;
            }
        }
    } else {
        // Digital sensor implementation would go here
        // For now, just mark as not implemented
        g_sensor_data.status = SENSOR_STATUS_ERROR;
    }
    
    return g_sensor_data;
}

// Get current sensor status
sensor_status_t sensor_get_status(void) 
{
    return g_sensor_data.status;
}

// Get sensor configuration
const sensor_config_t* sensor_get_config(void) 
{
    return g_module_initialized ? &g_sensor_config : nullptr;
}

// Convert raw ADC value (0-1023) to voltage (0-5V)
float sensor_raw_to_voltage(uint16_t raw_value) 
{
    // Arduino Mega uses 10-bit ADC: 0-1023 maps to 0-5V
    const float ADC_RESOLUTION = 1023.0f;
    const float REFERENCE_VOLTAGE = 5.0f;
    
    return (raw_value / ADC_RESOLUTION) * REFERENCE_VOLTAGE;
}

// Convert voltage to percentage (0-100%)
float sensor_voltage_to_percentage(float voltage) 
{
    const float max_voltage = g_sensor_config.max_voltage;
    const float min_voltage = g_sensor_config.min_voltage;
    
    if (max_voltage <= min_voltage) return 0.0f;
    
    // Clamp voltage to valid range
    if (voltage < min_voltage) voltage = min_voltage;
    if (voltage > max_voltage) voltage = max_voltage;
    
    // Calculate percentage
    return ((voltage - min_voltage) / (max_voltage - min_voltage)) * 100.0f;
}

// Check if sensor voltage value is within expected range
bool sensor_is_value_valid(float voltage) 
{
    return (voltage >= g_sensor_config.min_voltage && 
            voltage <= g_sensor_config.max_voltage);
}

// Reset error counter
void sensor_reset_errors(void) 
{
    g_sensor_data.error_count = 0;
    if (g_sensor_data.status == SENSOR_STATUS_ERROR) {
        g_sensor_data.status = SENSOR_STATUS_OK;
    }
}

// Convert voltage to temperature using NTC thermistor equation (Steinhart-Hart approximation)
float sensor_voltage_to_temperature(float voltage) 
{
    // Avoid division by zero and invalid voltages
    if (voltage <= 0.1f || voltage >= 4.9f) {
        return 25.0f; // Return default temperature for invalid readings
    }
    
    // For Wokwi NTC sensor: simplified approach
    // Wokwi NTC works with voltage divider: Vout = 5V * (R_ntc / (R_pullup + R_ntc))
    // Solving for R_ntc: R_ntc = R_pullup * Vout / (5V - Vout)
    const float Vin = 5.0f;
    float denominator = Vin - voltage;
    
    // Prevent division by very small numbers
    if (denominator < 0.1f) {
        denominator = 0.1f;
    }
    
    float R_ntc = TEMP_PULLUP_RESISTOR * voltage / denominator;
    
    // Prevent invalid resistance values
    if (R_ntc <= 0.0f || R_ntc > 1000000.0f) {
        return 25.0f; // Return room temperature for invalid resistance
    }
    
    // Simplified Steinhart-Hart equation for NTC: 1/T = 1/T0 + (1/B) * ln(R/R25)
    // Where: T - temperature in Kelvin, T0 - reference temp (298.15K for 25°C), 
    //        B - beta coefficient, R - current resistance, R25 - resistance at 25°C
    float ratio = R_ntc / TEMP_SENSOR_R25;
    
    // Prevent log of zero or negative numbers
    if (ratio <= 0.0f) {
        return 25.0f;
    }
    
    float ln_ratio = log(ratio);
    float temp_kelvin = 1.0f / ((1.0f / TEMP_SENSOR_T0) + (ln_ratio / TEMP_SENSOR_BETA));
    
    // Convert Kelvin to Celsius
    float temp_celsius = temp_kelvin - 273.15f;
    
    // Validate reasonable temperature range
    if (temp_celsius < -50.0f || temp_celsius > 150.0f) {
        return 25.0f; // Return room temperature for out-of-range values
    }
    
    return temp_celsius;
}

// Convert temperature to normalized percentage for display (0-100%)
float sensor_temperature_to_percentage(float temperature) 
{
    const float min_temp = TEMP_MIN_CELSIUS;
    const float max_temp = TEMP_MAX_CELSIUS;
    
    if (max_temp <= min_temp) return 0.0f;
    
    // Clamp temperature to valid range
    if (temperature < min_temp) temperature = min_temp;
    if (temperature > max_temp) temperature = max_temp;
    
    // Calculate percentage
    return ((temperature - min_temp) / (max_temp - min_temp)) * 100.0f;
}

// Check if temperature is within expected range
bool sensor_is_temperature_valid(float temperature) 
{
    // Accept a wider range for validation, as we now return default values for errors
    return (temperature >= -55.0f && temperature <= 130.0f && temperature != 25.0f); // 25.0 is our error default
}

// Generate formatted sensor report string
void sensor_get_report(char* buffer, size_t buffer_size) 
{
    if (buffer == nullptr || buffer_size == 0) return;
    
    if (!g_module_initialized) {
        snprintf(buffer, buffer_size, "SENSOR: Not initialized");
        return;
    }
    
    const char* status_str;
    switch (g_sensor_data.status) {
        case SENSOR_STATUS_OK:
            status_str = "OK";
            break;
        case SENSOR_STATUS_ERROR:
            status_str = "ERROR";
            break;
        case SENSOR_STATUS_OUT_OF_RANGE:
            status_str = "OUT_OF_RANGE";
            break;
        case SENSOR_STATUS_NOT_CONNECTED:
            status_str = "NOT_CONNECTED";
            break;
        default:
            status_str = "UNKNOWN";
            break;
    }
    
    float temperature = g_sensor_data.physical_value; // Now stores temperature in Celsius
    float percentage = sensor_temperature_to_percentage(temperature);
    
    // Convert float to int for Arduino printf compatibility
    int temp_int = (int)(temperature * 10.0f); // Temperature * 10 for one decimal place
    int perc_int = (int)(percentage * 10.0f);  // Percentage * 10 for one decimal place
    
    snprintf(buffer, buffer_size, 
        "%s: %d.%d°C (%d.%d%%) [RAW:%d] [%s] [ERR:%d] [TIME:%lu]",
        g_sensor_config.name,
        temp_int / 10, temp_int % 10,        // Split integer for decimal display
        perc_int / 10, perc_int % 10,        // Split integer for decimal display  
        g_sensor_data.raw_value,
        status_str,
        g_sensor_data.error_count,
        g_sensor_data.timestamp_ms
    );
}
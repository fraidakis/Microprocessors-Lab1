/*
 * ===========================================================================
 * File:        dht11.c
 * Description: DHT11 temperature and humidity sensor function implementations
 * Date:        May, 2025
 * ===========================================================================
 */

/* ========================================================================== */
/*                              INCLUDES                                      */
/* ========================================================================== */
#include "dht11.h"
#include "system_config.h"
#include "gpio.h"
#include "delay.h"


/* ========================================================================== */
/*                        PRIVATE FUNCTION DECLARATIONS                       */
/* ========================================================================== */
static uint8_t dht11_send_start_signal(void);
static uint8_t dht11_wait_for_response(void);
static uint8_t dht11_read_data_bits(uint8_t *data);
static uint8_t dht11_verify_checksum(uint8_t *data);
static void dht11_convert_to_float(uint8_t *data, float *temperature, float *humidity);


/* ========================================================================== */
/*                        DHT11 SENSOR FUNCTIONS                             */
/* ========================================================================== */

/**
 * @brief Read temperature and humidity from DHT11 sensor
 */
uint8_t read_DHT11_Sensor(float *temperature, float *humidity)
{
    uint8_t data[5] = {0};
    
    // Disable interrupts for entire DHT11 communication sequence
    __disable_irq();
    
    // Send start signal to DHT11
    if (!dht11_send_start_signal()) {
        __enable_irq();
        return 0;
    }
    
    // Wait for DHT11 response
    if (!dht11_wait_for_response()) {
        __enable_irq();
        return 0;
    }
    
    // Read 40 bits of data
    if (!dht11_read_data_bits(data)) {
        __enable_irq();
        return 0;
    }
    
    // Re-enable interrupts after critical timing sections
    __enable_irq();
    
    // Verify checksum (non-timing critical)
    if (!dht11_verify_checksum(data)) {
        return 0;
    }
    
    // Convert to float values (non-timing critical)
    dht11_convert_to_float(data, temperature, humidity);
    
    return 1;
}


/* ========================================================================== */
/*                        PRIVATE FUNCTION IMPLEMENTATIONS                    */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*                    DHT11 Communication Protocol                            */
/* -------------------------------------------------------------------------- */

/**
 * @brief Send start signal to DHT11 sensor
 * 
 * @return uint8_t Success status (always 1)
 * 
 * Initiates communication with DHT11 by sending the required start signal:
 * - Pull data line LOW for 18ms
 * - Pull data line HIGH for 30us
 * - Release line for sensor response
 */
static uint8_t dht11_send_start_signal(void)
{
    gpio_set_mode(DHT11_PIN, Output);
    gpio_set(DHT11_PIN, OFF);
    delay_ms(18);
    gpio_set(DHT11_PIN, ON);
    delay_us(30);
    gpio_set_mode(DHT11_PIN, PullUp);
    return 1;
}

/**
 * @brief Wait for DHT11 sensor response signal
 * 
 * @return uint8_t Success status (1 if response received, 0 if timeout)
 * 
 * Waits for DHT11 response sequence:
 * - Wait for pin to go LOW (80us)
 * - Wait for pin to go HIGH (80us) 
 * - Wait for pin to go LOW again (start of data transmission)
 */
static uint8_t dht11_wait_for_response(void)
{
    uint32_t timeout;
    
    // Wait for pin to go LOW
    timeout = 1000;
    while (gpio_get(DHT11_PIN) && timeout--);
    if (timeout == 0) return 0;
    
    // Wait for pin to go HIGH
    timeout = 1000;
    while (!gpio_get(DHT11_PIN) && timeout--);
    if (timeout == 0) return 0;
    
    // Wait for pin to go LOW again (start of data)
    timeout = 1000;
    while (gpio_get(DHT11_PIN) && timeout--);
    if (timeout == 0) return 0;
    
    return 1;
}

/* -------------------------------------------------------------------------- */
/*                    DHT11 Data Processing                                   */
/* -------------------------------------------------------------------------- */

/**
 * @brief Read 40 bits of data from DHT11 sensor
 * 
 * @param data Pointer to 5-byte array to store received data
 * @return uint8_t Success status (1 if successful, 0 if timeout)
 * 
 * Reads 40 bits of data (5 bytes) from DHT11 sensor:
 * - Byte 0: Humidity integer part
 * - Byte 1: Humidity decimal part  
 * - Byte 2: Temperature integer part
 * - Byte 3: Temperature decimal part
 * - Byte 4: Checksum
 */
static uint8_t dht11_read_data_bits(uint8_t *data)
{
    uint32_t timeout;
    
    for (uint8_t byte_count = 0; byte_count < 5; byte_count++) {
        for (uint8_t bit_count = 0; bit_count < 8; bit_count++) {
            // Wait for bit start (LOW -> HIGH transition)
            timeout = 1000;
            while (!gpio_get(DHT11_PIN) && timeout--);
            if (timeout == 0) return 0;
            
            // Determine bit value based on HIGH pulse duration
            delay_us(30);
            if (gpio_get(DHT11_PIN)) {
                // Long HIGH pulse = bit 1
                data[byte_count] |= (1 << (7 - bit_count));
                
                // Wait for bit end (HIGH to LOW)
                timeout = 1000;
                while (gpio_get(DHT11_PIN) && timeout--);
                if (timeout == 0) return 0;
            }
            // Short HIGH pulse = bit 0 (no action needed)
        }
    }
    return 1;
}

/**
 * @brief Verify DHT11 data integrity using checksum
 * 
 * @param data Pointer to 5-byte data array
 * @return uint8_t Verification status (1 if valid, 0 if invalid)
 * 
 * Validates data integrity by comparing calculated checksum (sum of first
 * 4 bytes) against the checksum byte provided by the sensor.
 */
static uint8_t dht11_verify_checksum(uint8_t *data)
{
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    return (checksum == data[4]);
}

/**
 * @brief Convert raw DHT11 data to floating-point values
 * 
 * @param data        Pointer to 5-byte raw data array
 * @param temperature Pointer to store converted temperature value
 * @param humidity    Pointer to store converted humidity value
 * 
 * Converts raw DHT11 sensor data to human-readable floating-point values:
 * - Humidity: integer + decimal/10 (percentage)
 * - Temperature: integer + decimal/10 (Celsius)
 */
static void dht11_convert_to_float(uint8_t *data, float *temperature, float *humidity)
{
    *humidity = (float)data[0] + (float)data[1] / 10.0f;
    *temperature = (float)data[2] + (float)data[3] / 10.0f;
}
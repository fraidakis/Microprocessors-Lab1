/*
 * ===========================================================================
 * File:        dht11.h
 * Description: DHT11 temperature and humidity sensor function declarations
 * Date:        May, 2025
 * ===========================================================================
 */

#ifndef DHT11_H
#define DHT11_H

#include <stdint.h>


/* ========================================================================== */
/*                        DHT11 SENSOR FUNCTIONS                             */
/* ========================================================================== */

/* -------------------------------------------------------------------------- */
/*                    Temperature and Humidity Reading                        */
/* -------------------------------------------------------------------------- */

/**
 * @brief Read temperature and humidity from DHT11 sensor
 * 
 * @param temperature Pointer to store temperature value in Celsius
 * @param humidity    Pointer to store humidity value as percentage
 * @return uint8_t    Success status (1 if successful, 0 if failed)
 * 
 * Performs complete DHT11 sensor reading sequence including start signal
 * transmission, response validation, data bit reading, checksum verification,
 * and conversion to floating-point temperature and humidity values.
 */
uint8_t read_DHT11_Sensor(float *temperature, float *humidity);

#endif // DHT11_H
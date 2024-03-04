#ifndef SENSOR_H
#define SENSOR_H

#include "config.h"
#include "ds18b20.h"
#include "onewire_bus.h"

/**
 * @brief Initializes a one-wire bus on the given pin.
 *
 * @param pin The pin number for the bus.
 * @param bus_handle Pointer to store the initialized bus handle.
 * @return ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t init_sensor_bus(int pin, onewire_bus_handle_t *bus_handle);

#ifdef CONFIG_ESP_SCANNER_MODE

/**
 * @brief Perform a sensor scan on the given 1-Wire bus and
 * print the results to the serial console.
 *
 * @param bus the 1-Wire bus handle.
 */
void sensor_scan(onewire_bus_handle_t bus);

#else

/**
 * @brief Initializes a 1-Wire device with the given address on the bus.
 *
 * @param bus the 1-Wire bus handle.
 * @param address the 1-Wire device address.
 * @param device_handle pointer to store the initialized device handle.
 * @return ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t init_onewire_device(onewire_bus_handle_t bus, uint64_t address,
                              onewire_device_t *device_handle);

/**
 * @brief Initializes a DS18B20 sensor with the given address on the bus.
 *
 * @param bus the 1-Wire bus handle.
 * @param address the 1-Wire device address.
 * @param device_handle pointer to store the initialized device handle.
 * @param sensor_handle pointer to store the initialized sensor handle.
 * @return ESP_OK if initialization is successful, otherwise an error code.
 */
esp_err_t init_ds18b20_sensor(onewire_bus_handle_t bus, uint64_t address,
                              onewire_device_t *device_handle,
                              ds18b20_device_handle_t *sensor_handle);

/**
 * @brief Converts an integer to a `ds18b20_resolution_t` value.
 *
 * @param resolution the integer value to convert.
 * @return ds18b20_resolution_t the converted resolution value.
 */
ds18b20_resolution_t int_to_resolution(int resolution);

/**
 * @brief Get the maximum conversion time in milliseconds for the given
 * resolution.
 *
 * @param resolution the resolution value to convert.
 * @return float the maximum conversion time in milliseconds.
 */
float ds18b20_max_conversion_time_ms(ds18b20_resolution_t resolution);

#endif // CONFIG_ESP_SCANNER_MODE

#endif // SENSOR_H

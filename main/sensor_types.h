#ifndef SENSOR_TYPES_H
#define SENSOR_TYPES_H

#include <stdint.h>

#ifndef CONFIG_ESP_SCANNER_MODE

/**
 * @brief A struct to hold a sensor reading.
 */
typedef struct {
  int idx;           /**< The index of the sensor. */
  float temperature; /**< The temperature reading. */
} sensor_reading_t;

#endif // CONFIG_ESP_SCANNER_MODE

#endif // SENSOR_TYPES_H
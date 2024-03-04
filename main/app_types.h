#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "config_types.h"

#include "onewire_device.h"

#ifndef CONFIG_ESP_SCANNER_MODE
#include "ds18b20.h"
#include "sensor_types.h"
#endif // CONFIG_ESP_SCANNER_MODE

typedef struct {
  int code;
  const char *message;
} app_error_t;

typedef struct {
#ifdef CONFIG_ESP_DEBUG_MODE
  struct timespec start_time;
#endif // CONFIG_ESP_DEBUG_MODE
  bool running;
  app_error_t *errors;
  uint8_t num_errors;
  onewire_config_t onewire_config;
  onewire_bus_handle_t *bus_handles;
  uint8_t num_buses;
#ifndef CONFIG_ESP_SCANNER_MODE
  sensor_reading_t *sensor_readings;
  onewire_device_t *device_handles;
  ds18b20_device_handle_t *sensor_handles;
  uint8_t num_sensors;
  mqtt_config_t mqtt_config;
#endif // CONFIG_ESP_SCANNER_MODE

} app_state_t;

#endif // APP_TYPES_H
#include "sensor.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

static const char *TAG = "sensor";

esp_err_t init_sensor_bus(int pin, onewire_bus_handle_t *bus_handle) {
  if (pin < 0) {
    ESP_LOGE(TAG, "Invalid pin number: %d", pin);
    return ESP_ERR_INVALID_ARG;
  }

  if (bus_handle == NULL) {
    ESP_LOGE(TAG, "Bus handle cannot be NULL");
    return ESP_ERR_INVALID_ARG;
  }

  ESP_LOGD(TAG, "Initializing 1-Wire bus on GPIO %d", pin);

  onewire_bus_config_t bus_config = {.bus_gpio_num = pin};
  onewire_bus_rmt_config_t rmt_config = {
      .max_rx_bytes =
          10 // 1byte ROM command + 8byte ROM number + 1byte device command
  };

  esp_err_t err = onewire_new_bus_rmt(&bus_config, &rmt_config, bus_handle);

  if (err != ESP_OK) {
    return err;
  }

  return ESP_OK;
}

#ifdef CONFIG_ESP_SCANNER_MODE

void sensor_scan(onewire_bus_handle_t bus) {
  onewire_device_iter_handle_t iter = NULL;
  onewire_device_t device;
  esp_err_t search_result;

  ESP_ERROR_CHECK(onewire_new_device_iter(bus, &iter));

  ESP_LOGI(TAG, "Starting OneWire device scan...");
  while ((search_result = onewire_device_iter_get_next(iter, &device)) ==
         ESP_OK) {
    ESP_LOGI(TAG, "Found device with address: %016llX", device.address);
  }
  ESP_ERROR_CHECK(onewire_del_device_iter(iter));

  if (search_result == ESP_ERR_NOT_FOUND) {
    ESP_LOGI(TAG, "No more devices found");
  } else {
    ESP_LOGE(TAG, "Error occurred during device scan: %d", search_result);
  }
}

#else // CONFIG_ESP_SCANNER_MODE

esp_err_t init_onewire_device(onewire_bus_handle_t bus, uint64_t address,
                              onewire_device_t *device_handle) {
  if (device_handle == NULL) {
    ESP_LOGE(TAG, "Device handle cannot be NULL");
    return ESP_ERR_INVALID_ARG;
  }

  ESP_LOGD(TAG, "Initializing 1-Wire device with address: %016llX", address);

  *device_handle = (onewire_device_t){
      .bus = bus,
      .address = address,
  };

  return ESP_OK;
}

esp_err_t init_ds18b20_sensor(onewire_bus_handle_t bus, uint64_t address,
                              onewire_device_t *device_handle,
                              ds18b20_device_handle_t *sensor_handle) {
  if (sensor_handle == NULL) {
    ESP_LOGE(TAG, "Sensor handle cannot be NULL");
    return ESP_ERR_INVALID_ARG;
  }

  ESP_LOGD(TAG, "Initializing DS18B20 sensor with address: %016llX", address);

  ds18b20_config_t config = {};

  esp_err_t err = ds18b20_new_device(device_handle, &config, sensor_handle);

  if (err != ESP_OK) {
    return err;
  }

  return ESP_OK;
}

ds18b20_resolution_t int_to_resolution(int resolution) {
  switch (resolution) {
  case 9:
    return DS18B20_RESOLUTION_9B;
  case 10:
    return DS18B20_RESOLUTION_10B;
  case 11:
    return DS18B20_RESOLUTION_11B;
  case 12:
    return DS18B20_RESOLUTION_12B;
  default:
    ESP_LOGE(TAG, "Invalid resolution: %d", resolution);
    return -1;
  }
}

float ds18b20_max_conversion_time_ms(ds18b20_resolution_t resolution) {
  switch (resolution) {
  case DS18B20_RESOLUTION_9B:
    return 93.75;
  case DS18B20_RESOLUTION_10B:
    return 187.5;
  case DS18B20_RESOLUTION_11B:
    return 375;
  case DS18B20_RESOLUTION_12B:
    return 750;
  default:
    ESP_LOGE(TAG, "Invalid resolution: %d", resolution);
    return -1;
  }
}

#endif // CONFIG_ESP_SCANNER_MODE

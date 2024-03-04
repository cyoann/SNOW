#include "app.h"

#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "time.h"

static const char *TAG = "snow_app";

void app_init(app_state_t *state) {
  ESP_LOGI(TAG, "Initializing application");

#ifdef CONFIG_ESP_DEBUG_MODE
  clock_gettime(CLOCK_REALTIME, &state->start_time);
#endif

  // Initialize the application state
  state->running = true;
  state->errors = NULL;
  state->num_errors = 0;
  state->bus_handles = NULL;
  state->num_buses = 0;

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

#ifdef CONFIG_ESP_SLEEP_MODE
  ESP_LOGI(TAG, "Configuring deep sleep mode for %d seconds",
           CONFIG_ESP_SLEEP_DURATION);
  esp_sleep_enable_timer_wakeup(CONFIG_ESP_SLEEP_DURATION *
                                1000000); // seconds to microseconds
#endif

#ifdef CONFIG_ESP_DEBUG_MODE
  clock_gettime(CLOCK_REALTIME, &state->start_time);
  esp_log_level_set("*", ESP_LOG_DEBUG);
  ESP_LOGI(TAG, "Debug output enabled");
#endif

#ifndef CONFIG_ESP_SCANNER_MODE
  ESP_LOGI(TAG, "Parsing one-wire configuration");

  onewire_config_t *onewire_config =
      parse_onewire_config(CONFIG_ESP_ONE_WIRE_CONFIG_STRING);

  if (onewire_config == NULL) {
    app_append_error(state, 1, "Failed to parse one-wire configuration");
    state->running = false;
    return;
  }

  state->onewire_config = *onewire_config;

  ESP_LOGD(TAG, "Configuration:");
  ESP_LOGD(TAG, "OneWire buses (%d):", state->onewire_config.bus_count);
  for (int i = 0; i < state->onewire_config.bus_count; i++) {
    ESP_LOGD(TAG, "  Bus %d (GPIO: %d)", i,
             state->onewire_config.buses[i]->pin);
    for (int j = 0; j < state->onewire_config.buses[i]->sensor_count; j++) {
      sensor_config_t *sensor = state->onewire_config.buses[i]->sensors[j];
      ESP_LOGD(TAG, "    Sensor %d (address: %s, idx: %d, resolution: %d)", j,
               sensor->address, sensor->idx, sensor->resolution);
    }
  }

  ESP_LOGI(TAG, "Parsing MQTT configuration");
  mqtt_config_t mqtt_config;
  mqtt_config.broker_count = 0;
  mqtt_config.broker_capacity = 1;
  mqtt_config.brokers =
      malloc(sizeof(mqtt_broker_config_t) * mqtt_config.broker_capacity);
  parse_mqtt_connection_string(CONFIG_ESP_MQTT_CONNECTION_STRING, &mqtt_config);

  state->mqtt_config = mqtt_config;

  ESP_LOGD(TAG, "Brokers (%d):", mqtt_config.broker_count);
  for (int i = 0; i < mqtt_config.broker_count; i++) {
    mqtt_broker_config_t *broker = &mqtt_config.brokers[i];
    ESP_LOGD(TAG, " - Broker %d:", i);
    ESP_LOGD(TAG, "    - Protocol: %s",
             broker->protocol ? broker->protocol : "NULL");
    ESP_LOGD(TAG, "    - Host: %s", broker->host ? broker->host : "NULL");
    ESP_LOGD(TAG, "    - Port: %d", broker->port);
    ESP_LOGD(TAG, "    - Username: %s",
             broker->username ? broker->username : "NULL");
    ESP_LOGD(TAG, "    - Password: %s",
             broker->password ? broker->password : "NULL");
    ESP_LOGD(TAG, "    - Client ID: %s",
             broker->client_id ? broker->client_id : "NULL");
    ESP_LOGD(TAG, "    - Topic: %s", broker->topic ? broker->topic : "NULL");
  }

#endif

  int adc_reading = adc1_get_raw(ADC1_CHANNEL_0);
  ESP_LOGI(TAG, "ADC1 CH0: %d", adc_reading);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  adc_reading = adc1_get_raw(ADC1_CHANNEL_3);
  ESP_LOGI(TAG, "ADC1 CH3: %d", adc_reading);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  adc_reading = adc1_get_raw(ADC1_CHANNEL_6);
  ESP_LOGI(TAG, "ADC1 CH6: %d", adc_reading);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  adc_reading = adc1_get_raw(ADC1_CHANNEL_7);
  ESP_LOGI(TAG, "ADC1 CH7: %d", adc_reading);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  adc_reading = adc1_get_raw(ADC1_CHANNEL_4);
  ESP_LOGI(TAG, "ADC1 CH4: %d", adc_reading);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  adc_reading = adc1_get_raw(ADC1_CHANNEL_5);
  ESP_LOGI(TAG, "ADC1 CH5: %d", adc_reading);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  adc_reading = adc1_get_raw(ADC1_CHANNEL_1);
  ESP_LOGI(TAG, "ADC1 CH1: %d", adc_reading);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void app_run(app_state_t *state) {
  ESP_LOGI(TAG, "Running application");

// SCANNER MODE
#ifdef CONFIG_ESP_SCANNER_MODE
  run_scanner_mode(state);
#else // CONFIG_ESP_SCANNER_MODE
  run_normal_mode(state);

#endif // CONFIG_ESP_SCANNER_MODE
}

#ifdef CONFIG_ESP_SCANNER_MODE
void run_scanner_mode(app_state_t *state) {
#ifdef CONFIG_ESP_SIMULATED_DEVICE
  ESP_LOGI(TAG, "Running in simulation mode");
  char *addresses[] = {"28FF1A4E7A1904A1", "28FF1A4E7A1904A2",
                       "28FF1A4E7A1904A3", "28FF1A4E7A1904A4"};
  for (int i = 0; i < 4; i++) {
    ESP_LOGI(TAG, "Found device with address: %s", addresses[i]);
  }
  state->running = false;
  return;
#else
  ESP_LOGI(TAG, "Running in scanner mode");
#ifdef CONFIG_ESP_SCANNER_GPIO
  char delimiter = ','; // Delimiter used to separate the numbers
  int numbers[32];      // Array to store the converted numbers
  int num_count;        // Variable to store the number of integers converted

  // Convert the string to an array of integers
  num_count = str_to_int_array(CONFIG_ESP_SCANNER_GPIO, delimiter, numbers, 32);

  if (num_count == -1 || num_count == 0) {
    app_append_error(state, 2,
                     "Failed to convert GPIO string to array of integers");
    state->running = false;
    return;
  }

  state->bus_handles = malloc(num_count * sizeof(onewire_bus_handle_t));
  state->num_buses = num_count;
  for (int i = 0; i < num_count; i++) {
    esp_err_t err = init_sensor_bus(numbers[i], &state->bus_handles[i]);
    if (err != ESP_OK) {
      app_append_error(state, 3, "Failed to initialize 1-Wire bus");
      state->running = false;
      return;
    }

    sensor_scan(state->bus_handles[i]);
  }
#else  // CONFIG_ESP_SCANNER_GPIO
  app_append_error(state, 4, "Scanner mode requires GPIO pins to be specified");
#endif // CONFIG_ESP_SCANNER_GPIO

  state->running = false;
  return;
#endif // CONFIG_ESP_SIMULATED_DEVICE
}
#endif // CONFIG_ESP_SCANNER_MODE

#ifndef CONFIG_ESP_SCANNER_MODE
void run_normal_mode(app_state_t *state) {

#ifdef CONFIG_ESP_SIMULATED_DEVICE
  ESP_LOGI(TAG, "Running in simulation mode");

  // Simulate the sensor readings
  state->num_sensors = 4;
  state->sensor_readings =
      malloc(state->num_sensors * sizeof(sensor_reading_t));

  for (int i = 0; i < state->num_sensors; i++) {
    state->sensor_readings[i].idx = i;
    state->sensor_readings[i].temperature = 20.0 + i;

    ESP_LOGI(TAG, "Sensor %d temperature: %.2f C", i,
             state->sensor_readings[i].temperature);
  }

  // Connect to Wi-Fi
  ESP_LOGI(TAG, "Connecting to Wi-Fi");
  esp_err_t err = wifi_init_sta();

  if (err != ESP_OK) {
    app_append_error(state, 7, "Failed to connect to Wi-Fi. Exiting.");
    state->running = false;
    return;
  }

  // Publish the sensor readings to the configured MQTT brokers
  ESP_LOGI(TAG, "Publishing sensor readings to MQTT brokers");
  publish_sensor_readings(state);

#else

  if (state->bus_handles == NULL) {
    init_onewire_buses(state);
  }

  if (state->num_sensors == 0) {
    calculate_num_sensors(state);
  }

  read_sensors(state);

  if (state->num_errors > 0) {
    log_errors(state);
  } else {

    // Connect to Wi-Fi
    ESP_LOGI(TAG, "Connecting to Wi-Fi");
    esp_err_t err = wifi_init_sta();
    if (err != ESP_OK) {
      app_append_error(state, 7, "Failed to connect to Wi-Fi. Exiting.");
      state->running = false;
      return;
    }

    publish_sensor_readings(state);
  }

#ifdef CONFIG_ESP_SLEEP_MODE
  state->running = false;
#else
  vTaskDelay(CONFIG_ESP_SLEEP_DURATION * 1000 / portTICK_PERIOD_MS);
#endif

#endif
}

void init_onewire_buses(app_state_t *state) {
  ESP_LOGI(TAG, "Initializing 1-Wire buses");

  state->bus_handles =
      malloc(state->onewire_config.bus_count * sizeof(onewire_bus_handle_t));
  state->num_buses = state->onewire_config.bus_count;

  for (int i = 0; i < state->onewire_config.bus_count; i++) {
    esp_err_t err = init_sensor_bus(state->onewire_config.buses[i]->pin,
                                    &state->bus_handles[i]);
    if (err != ESP_OK) {
      app_append_error(state, 2, "Failed to initialize 1-Wire bus");
      state->running = false;
      return;
    }
  }

  ESP_LOGI(TAG, "1-Wire buses initialized");
}

void calculate_num_sensors(app_state_t *state) {
  ESP_LOGI(TAG, "Calculating the number of sensors");

  state->num_sensors = 0;
  for (int i = 0; i < state->onewire_config.bus_count; i++) {
    state->num_sensors += state->onewire_config.buses[i]->sensor_count;
  }

  ESP_LOGI(TAG, "Number of sensors: %d", state->num_sensors);
}

void read_sensors(app_state_t *state) {
  ESP_LOGI(TAG, "Reading the sensors");

  state->sensor_readings =
      malloc(state->num_sensors * sizeof(sensor_reading_t));
  state->device_handles = malloc(state->num_sensors * sizeof(onewire_device_t));
  state->sensor_handles =
      malloc(state->num_sensors * sizeof(ds18b20_device_handle_t));

  int current_sensor_reading_idx = 0;
  for (int i = 0; i < state->num_buses; i++) {
    for (int j = 0; j < state->onewire_config.buses[i]->sensor_count; j++) {
      sensor_config_t *sensor = state->onewire_config.buses[i]->sensors[j];
      current_sensor_reading_idx =
          i * state->onewire_config.buses[i]->sensor_count + j;

      onewire_device_t device;
      ds18b20_device_handle_t sensor_handle;

      esp_err_t err = init_onewire_device(
          state->bus_handles[i], strtoull(sensor->address, NULL, 16), &device);
      if (err != ESP_OK) {
        app_append_error(state, 3, "Failed to initialize 1-Wire device");
        // Skip to the next sensor
        continue;
      }

      err = init_ds18b20_sensor(state->bus_handles[i],
                                strtoull(sensor->address, NULL, 16), &device,
                                &sensor_handle);
      if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize sensor device");
        app_append_error(state, 4, "Failed to initialize sensor device");
        // Skip to the next sensor
        continue;
      }

      // Trigger a temperature conversion on the sensor
      err = ds18b20_trigger_temperature_conversion(sensor_handle);

      if (err != ESP_OK) {
        app_append_error(state, 5,
                         "Failed to trigger temperature conversion on sensor");

        // Skip to the next sensor
        continue;
      }

      // Wait for the conversion to complete
      float max_conversion_time =
          ds18b20_max_conversion_time_ms(int_to_resolution(sensor->resolution));
      ESP_LOGD(TAG, "Waiting for temperature conversion to complete (%.2f ms)",
               max_conversion_time);
      vTaskDelay(max_conversion_time / portTICK_PERIOD_MS);

      // Read the temperature from the sensor

      sensor_reading_t *reading =
          &state->sensor_readings[current_sensor_reading_idx];

      reading->idx = sensor->idx;
      err = ds18b20_get_temperature(sensor_handle, &reading->temperature);

      if (err != ESP_OK) {
        app_append_error(state, 5, "Failed to read sensor temperature");
      }

      // Log the sensor reading
      ESP_LOGI(TAG, "Sensor %d temperature: %.2f C", sensor->idx,
               reading->temperature);

#ifdef CONFIG_ESP_SLEEP_MODE
      err = ds18b20_del_device(sensor_handle);
      if (err != ESP_OK) {
        app_append_error(state, 6, "Failed to deinitialize sensor device");
      }
#endif
    }
  }
}

void publish_sensor_readings(app_state_t *state) {
  ESP_LOGI(TAG, "Publishing sensor readings to MQTT brokers");

  for (int i = 0; i < state->mqtt_config.broker_count; i++) {
    mqtt_broker_config_t *broker = &state->mqtt_config.brokers[i];
    MQTT_Client mqtt_client;
    esp_err_t err = mqtt_init(&mqtt_client, broker);
    if (err != ESP_OK) {
      app_append_error(state, 6, "Failed to initialize MQTT client");
      // Skip to the next broker
      continue;
    }
    ESP_LOGI(TAG, "Connecting to MQTT broker %s", broker->host);
    err = mqtt_start(&mqtt_client);
    if (err != ESP_OK) {
      app_append_error(state, 7, "Failed to connect to MQTT broker");
      // Skip to the next broker
      continue;
    }
    ESP_LOGI(TAG, "Publishing sensor readings to topic %s", broker->topic);

    for (int j = 0; j < state->num_sensors; j++) {
      char *reading_str;

#ifdef CONFIG_ESP_MQTT_DOMOTICZ_INTEGRATION
      asprintf(&reading_str,
               "{\"command\":\"udevice\", \"idx\":%d, \"svalue\":\"%.2f\"}",
               state->sensor_readings[j].idx,
               state->sensor_readings[j].temperature);
#else
      asprintf(&reading_str,
               "{\"address\":\"%s\", \"idx\":%d, \"temperature\":%.2f}",
               state->onewire_config.buses[j]->sensors[j]->address,
               state->sensor_readings[j].idx,
               state->sensor_readings[j].temperature);
#endif

      mqtt_publish(&mqtt_client, broker, reading_str);
      free(reading_str);
    }
  }

  // Free the sensor readings
  free(state->sensor_readings);
  state->sensor_readings = NULL;
}
#endif // CONFIG_ESP_SCANNER_MODE

void log_errors(app_state_t *state) {
  for (int i = 0; i < state->num_errors; i++) {
    ESP_LOGE(TAG, "Error %d: %s", state->errors[i].code,
             state->errors[i].message);
  }
}

void free_errors(app_state_t *state) {
  if (state->errors != NULL) {
    free(state->errors);
  }
}

void log_runtime(app_state_t *state) {
#ifdef CONFIG_ESP_DEBUG_MODE
  struct timespec end_time;
  clock_gettime(CLOCK_REALTIME, &end_time);
  int runtime = (end_time.tv_sec - state->start_time.tv_sec) * 1000000000 +
                (end_time.tv_nsec - state->start_time.tv_nsec);

  ESP_LOGD(TAG, "Application runtime: %d ns (%.3f s)", runtime,
           runtime / 1000000000.0);
#endif
}

void enter_sleep_mode() {
#ifdef CONFIG_ESP_SLEEP_MODE
#ifdef CONFIG_ESP_SIMULATED_DEVICE
  ESP_LOGI(TAG, "Simulating deep sleep mode for %d seconds",
           CONFIG_ESP_SLEEP_DURATION);
  vTaskDelay(CONFIG_ESP_SLEEP_DURATION * 1000 / portTICK_PERIOD_MS);
  esp_restart();
#else
  ESP_LOGI(TAG, "Entering deep sleep mode");
  esp_deep_sleep_start();
#endif
#endif
}

void app_deinit(app_state_t *state) {
  ESP_LOGI(TAG, "Deinitializing application");

  // Log the errors
  log_errors(state);

  // Log the runtime
  log_runtime(state);

  // Deinitialize the application
  app_free_state(state);

  // Configure the ESP32 to enter deep sleep mode
  enter_sleep_mode();
}

void app_append_error(app_state_t *state, int code, const char *message) {
  app_error_t error = {
      .code = code,
      .message = message,
  };

  if (state->errors == NULL) {
    state->errors = malloc(sizeof(app_error_t));
  } else {
    state->errors =
        realloc(state->errors, (state->num_errors + 1) * sizeof(app_error_t));
  }
  state->errors[state->num_errors] = error;
  state->num_errors++;
}

void app_free_state(app_state_t *state) {
  free_errors(state);
  free(state->bus_handles);
#ifndef CONFIG_ESP_SCANNER_MODE
  free(state->device_handles);
  free(state->sensor_handles);
  free_onewire_config(&state->onewire_config);
  free_mqtt_config(&state->mqtt_config);

  if (state->sensor_readings != NULL) {
    free(state->sensor_readings);
  }
#endif
}
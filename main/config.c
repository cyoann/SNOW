#include "config.h"

#include <stdio.h>

#include "ds18b20_types.h"
#include "esp_log.h"

#include "utils.h"

static const char *TAG = "snow_config";

sensor_config_t *create_sensor_config(const char *address, int idx,
                                      int resolution) {
  if (address == NULL || idx < 0 || resolution < 0) {
    fprintf(stderr, "Invalid input parameters for create_sensor_config\n");
    return NULL;
  }

  sensor_config_t *sensor = (sensor_config_t *)malloc(sizeof(sensor_config_t));
  if (sensor == NULL) {
    fprintf(stderr, "Memory allocation failed for create_sensor_config\n");
    return NULL;
  }

  // Copy the address
  strncpy(sensor->address, address, MAX_SENSOR_ADDRESS_LENGTH - 1);
  sensor->address[MAX_SENSOR_ADDRESS_LENGTH - 1] = '\0';

  sensor->idx = idx;
  sensor->resolution = resolution;

  return sensor;
}

bus_config_t *create_bus_config(int pin) {
  if (pin < 0) {
    fprintf(stderr, "Invalid input parameter for create_bus_config\n");
    return NULL;
  }

  bus_config_t *bus = (bus_config_t *)malloc(sizeof(bus_config_t));
  if (bus == NULL) {
    fprintf(stderr, "Memory allocation failed for create_bus_config\n");
    return NULL;
  }

  bus->pin = pin;
  bus->sensors = NULL; // Initialize to NULL
  bus->sensor_count = 0;
  bus->sensor_capacity = 0;

  return bus;
}

void add_sensor_config_to_bus_config(bus_config_t *bus,
                                     sensor_config_t *sensor) {
  if (!bus || !sensor)
    return;

  if (bus->sensor_count >= bus->sensor_capacity) {
    int new_capacity =
        (bus->sensor_capacity == 0) ? 1 : bus->sensor_capacity * 2;
    sensor_config_t **new_sensors = (sensor_config_t **)realloc(
        bus->sensors, new_capacity * sizeof(sensor_config_t *));
    if (!new_sensors)
      return; // Failed to reallocate memory, return without adding the sensor
    bus->sensors = new_sensors;
    bus->sensor_capacity = new_capacity;
  }

  bus->sensors[bus->sensor_count++] = sensor;
}

void free_sensor_config(sensor_config_t *sensor) {
  if (sensor)
    free(sensor);
}

void free_bus_config(bus_config_t *bus) {
  if (!bus)
    return;

  for (int i = 0; i < bus->sensor_count; ++i) {
    free_sensor_config(bus->sensors[i]);
  }
  free(bus->sensors);
  free(bus);
}

onewire_config_t *create_onewire_config() {
  onewire_config_t *config =
      (onewire_config_t *)malloc(sizeof(onewire_config_t));
  if (config == NULL) {
    fprintf(stderr, "Memory allocation failed for create_onewire_config\n");
    return NULL;
  }

  config->buses = NULL; // Initialize to NULL
  config->bus_count = 0;
  config->bus_capacity = 0;

  return config;
}

void add_bus_to_onewire_config(onewire_config_t *config, bus_config_t *bus) {
  if (!config || !bus)
    return;

  if (config->bus_count >= config->bus_capacity) {
    int new_capacity =
        (config->bus_capacity == 0) ? 1 : config->bus_capacity * 2;
    bus_config_t **new_buses = (bus_config_t **)realloc(
        config->buses, new_capacity * sizeof(bus_config_t *));
    if (!new_buses)
      return; // Failed to reallocate memory, return without adding the bus
    config->buses = new_buses;
    config->bus_capacity = new_capacity;
  }

  config->buses[config->bus_count++] = bus;
}

void free_onewire_config(onewire_config_t *config) {
  if (!config)
    return;

  for (int i = 0; i < config->bus_count; ++i) {
    free_bus_config(config->buses[i]);
  }
  free(config->buses);
}

static bus_config_t *parse_bus_config(const char **ptr) {

  ESP_LOGD(TAG, "[parse_bus_config] input: %s", *ptr);

  int pin;
  if (sscanf(*ptr, "%d:", &pin) != 1) {
    // Failed to parse pin, skip this bus
    while (**ptr != ';' && **ptr != '\0') {
      (*ptr)++;
    }
    if (**ptr == ';') {
      (*ptr)++;
    }
    return NULL;
  }
  (*ptr)++; // Move past ':'

  // Create a new bus configuration
  bus_config_t *bus = create_bus_config(pin);
  if (!bus) {
    return NULL;
  }

  ESP_LOGD(TAG, "[parse_bus_config] bus created for pin: %d", pin);

  return bus;
}

static void parse_sensor_configs(const char **ptr, bus_config_t *bus) {
  ESP_LOGD(TAG, "[parse_sensor_configs] input: %s", *ptr);

  while (**ptr != ';' && **ptr != '\0') {
    skip_whitespace(ptr); // Skip any leading whitespace

    char address[MAX_SENSOR_ADDRESS_LENGTH];
    int idx;
    int resolution;

    if (sscanf(*ptr, "%17[^,],%d,%d|", address, &idx, &resolution) != 3) {
      // Failed to parse sensor, skip this sensor
      while (**ptr != '|' && **ptr != ';' && **ptr != '\0') {
        (*ptr)++;
      }
      if (**ptr == '|') {
        (*ptr)++;
      }
      continue;
    }

    sensor_config_t *sensor = create_sensor_config(address, idx, resolution);
    if (sensor) {
      add_sensor_config_to_bus_config(bus, sensor);
    }

    // Move to the next sensor
    while (**ptr != '|' && **ptr != ';' && **ptr != '\0') {
      (*ptr)++;
    }
    if (**ptr == '|') {
      (*ptr)++;
    }
  }

  ESP_LOGD(TAG, "[parse_sensor_configs] parsed %d sensors for bus %d",
           bus->sensor_count, bus->pin);
}

onewire_config_t *parse_onewire_config(const char *config_str) {
  if (!config_str)
    return NULL;

  ESP_LOGD(TAG, "[parse_onewire_config] input: %s", config_str);

  onewire_config_t *config = create_onewire_config();
  if (!config)
    return NULL;

  const char *ptr = config_str;

  // Iterate over each bus configuration
  while (*ptr != '\0') {
    bus_config_t *bus = parse_bus_config(&ptr);
    if (!bus) {
      continue;
    }

    // Move to the sensors part of the configuration
    while (*ptr != ':' && *ptr != '\0') {
      ptr++;
    }
    if (*ptr == '\0') {
      free_onewire_config(config);
      return NULL;
    }
    ptr++; // Move past ':'

    parse_sensor_configs(&ptr, bus);

    // Add the bus to the configuration
    add_bus_to_onewire_config(config, bus);

    // Move to the next bus or end of string
    while (*ptr != ';' && *ptr != '\0') {
      ptr++;
    }
    if (*ptr == ';') {
      ptr++;
    }
  }

  ESP_LOGD(TAG, "[parse_onewire_config] parsed %d buses", config->bus_count);

  return config;
}

#define MQTT_DEFAULT_PORT 1883
#define MQTT_DEFAULT_TLS_PORT 8883

void parse_mqtt_broker_connection_string(const char *connection_string,
                                         mqtt_broker_config_t *config) {
  assert(connection_string != NULL && config != NULL);

  ESP_LOGD(TAG, "[parse_mqtt_broker_connection_string] input: %s",
           connection_string);

  // Set default values
  config->protocol = NULL;
  config->host = NULL;
  config->port = -1; // Use -1 to indicate invalid port
  config->username = NULL;
  config->password = NULL;
  config->client_id = NULL;
  config->topic = NULL;

  // Copy the connection string to avoid modifying the original
  char *conn_copy = strdup(connection_string);
  if (conn_copy == NULL) {
    // Memory allocation failed
    return;
  }

  // Check if the connection string starts with "mqtt(s)://"
  char *protocol = strtok(conn_copy, ":/");
  if (protocol != NULL &&
      (strcmp(protocol, "mqtt") == 0 || strcmp(protocol, "mqtts") == 0)) {
    config->protocol = strdup(protocol);
    ESP_LOGD(TAG, "[parse_mqtt_broker_connection_string] protocol: %s",
             config->protocol);
  } else {
    ESP_LOGE(TAG, "Invalid protocol in connection string: %s",
             connection_string);
    free(conn_copy);
    return;
  }

  // Move past the protocol part of the connection string
  char *ptr = conn_copy + strlen(protocol) + 3;

  // Check if the connection string contains a username and password
  char *at = strchr(ptr, '@');
  if (at != NULL) {
    *at = '\0';
    char *colon = strchr(ptr, ':');
    if (colon != NULL) {
      *colon = '\0';
      config->username = strdup(ptr);
      config->password = strdup(colon + 1);

      ESP_LOGD(TAG, "[parse_mqtt_broker_connection_string] username: %s",
               config->username);
      ESP_LOGD(TAG, "[parse_mqtt_broker_connection_string] password: %s",
               config->password);
    }
    ptr = at + 1;
  }

  // Get the hostname part of the connection string
  char *slash = strchr(ptr, '/');
  if (slash != NULL) {
    *slash = '\0';
    config->host = strdup(ptr);
    ptr = slash + 1;
    ESP_LOGD(TAG, "[parse_mqtt_broker_connection_string] host: %s",
             config->host);
  } else {
    ESP_LOGE(TAG, "Invalid connection string: %s", connection_string);
    free(conn_copy);
    return;
  }

  // Check if the hostname contains a port
  char *colon = strchr(config->host, ':');
  if (colon != NULL) {
    *colon = '\0';
    config->port = atoi(colon + 1);
    ESP_LOGD(TAG, "[parse_mqtt_broker_connection_string] port: %d",
             config->port);
  } else {
    // Use default port based on protocol
    config->port = (strcmp(config->protocol, "mqtt") == 0)
                       ? MQTT_DEFAULT_PORT
                       : MQTT_DEFAULT_TLS_PORT;
    ESP_LOGD(TAG,
             "[parse_mqtt_broker_connection_string] using default port based "
             "on protocol (%s): %d",
             config->protocol, config->port);
  }

  // Check if there's a client ID
  char *question = strchr(ptr, '?');
  if (question != NULL) {
    *question = '\0';
    config->client_id = strdup(ptr);
    ptr = question + 1;
    ESP_LOGD(TAG, "[parse_mqtt_broker_connection_string] client_id: %s",
             config->client_id);
  } else {
    config->client_id = strdup(ptr);
    ESP_LOGD(TAG, "[parse_mqtt_broker_connection_string] client_id: %s",
             config->client_id);
    free(conn_copy);
    return;
  }

  // Check if there's a topic
  char *topic_prefix = "topic=";
  char *topic_ptr = strstr(ptr, topic_prefix);
  if (topic_ptr != NULL) {
    config->topic = strdup(topic_ptr + strlen(topic_prefix));
    ESP_LOGD(TAG, "[parse_mqtt_broker_connection_string] topic: %s",
             config->topic);
  }

  free(conn_copy);
}

void parse_mqtt_connection_string(const char *connection_string,
                                  mqtt_config_t *mqtt_config) {
  if (!connection_string || !mqtt_config)
    return;

  ESP_LOGD(TAG, "[parse_mqtt_connection_string] input: %s", connection_string);

  const char *ptr = connection_string;
  while (*ptr != '\0') {

    // Find the end of the current broker connection string
    const char *end = ptr;
    while (*end != ';' && *end != '\0') {
      end++;
    }

    // Create a temporary copy of the current broker connection string
    char *temp = strndup(ptr, end - ptr);
    if (!temp) {
      return;
    }

    // Parse the current broker connection string
    mqtt_broker_config_t config;
    parse_mqtt_broker_connection_string(temp, &config);

    // Add the parsed broker to the mqtt_config
    if (mqtt_config->broker_count >= mqtt_config->broker_capacity) {
      int new_capacity = (mqtt_config->broker_capacity == 0)
                             ? 1
                             : mqtt_config->broker_capacity * 2;
      mqtt_broker_config_t *new_brokers = (mqtt_broker_config_t *)realloc(
          mqtt_config->brokers, new_capacity * sizeof(mqtt_broker_config_t));
      if (!new_brokers) {
        free(temp);
        return; // Failed to reallocate memory, return without adding the broker
      }
      mqtt_config->brokers = new_brokers;
      mqtt_config->broker_capacity = new_capacity;
    }

    mqtt_config->brokers[mqtt_config->broker_count++] = config;

    // Move to the next broker connection string
    ptr = end;
    if (*ptr == ';') {
      ptr++;
    }

    free(temp);
  }

  ESP_LOGD(TAG, "[parse_mqtt_connection_string] parsed %d brokers",
           mqtt_config->broker_count);
}

void free_mqtt_broker_config(mqtt_broker_config_t *config) {
  if (config == NULL)
    return;

  // Free dynamically allocated members
  free(config->protocol);
  free(config->host);
  free(config->username);
  free(config->password);
  free(config->client_id);
  free(config->topic);

  // Set to NULL to avoid double free
  config->protocol = NULL;
  config->host = NULL;
  config->username = NULL;
  config->password = NULL;
  config->client_id = NULL;
  config->topic = NULL;
}

void free_mqtt_config(mqtt_config_t *mqtt_config) {
  if (mqtt_config == NULL)
    return;

  for (int i = 0; i < mqtt_config->broker_count; ++i) {
    free_mqtt_broker_config(&mqtt_config->brokers[i]);
  }
  free(mqtt_config->brokers);
  mqtt_config->brokers = NULL; // Set to NULL to avoid double free
}

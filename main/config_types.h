#ifndef CONFIG_TYPES_H
#define CONFIG_TYPES_H

#define MAX_SENSOR_ADDRESS_LENGTH                                              \
  18 ///< Maximum length of a sensor address (16 hex characters + 2 for null
     ///< terminator)

/**
 * @brief Represents configuration for a sensor.
 */
typedef struct {
  char address[MAX_SENSOR_ADDRESS_LENGTH]; ///< Sensor address
  int idx;                                 ///< Index of the sensor
  int resolution;                          ///< Resolution of the sensor
} sensor_config_t;

typedef struct {
  int pin;                   ///< Pin number for the bus
  sensor_config_t **sensors; ///< Array of pointers to SensorConfig
  int sensor_count;          ///< Number of sensors on the bus
  int sensor_capacity;       ///< Capacity of the sensors array
} bus_config_t;

typedef struct {
  bus_config_t **buses; ///< Array of pointers to BusConfig
  int bus_count;        ///< Number of buses
  int bus_capacity;     ///< Capacity of the buses array
} onewire_config_t;

/**
 * @brief Represents configuration for a MQTT broker.
 */
typedef struct {
  char *protocol;  ///< Protocol (e.g., "mqtt" or "mqtts")
  char *host;      ///< Hostname or IP address of the broker
  int port;        ///< Port number of the broker
  char *username;  ///< Username for authentication (if any)
  char *password;  ///< Password for authentication (if any)
  char *client_id; ///< Client ID for MQTT connection (if any)
  char *topic;     ///< Topic to publish sensor readings to
} mqtt_broker_config_t;

/**
 * @brief Represents configuration for MQTT connections.
 */
typedef struct {
  mqtt_broker_config_t *brokers; ///< Array of MQTT broker configurations
  int broker_count;              ///< Number of MQTT brokers
  int broker_capacity;           ///< Capacity of the brokers array
} mqtt_config_t;


#endif // CONFIG_TYPES_H
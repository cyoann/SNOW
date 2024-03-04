#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <string.h>

#include "config_types.h"

// onewire_config_t functions

/**
 * @brief Creates a new `sensor_config_t` instance.
 *
 * @param address The address of the sensor.
 * @param idx The index of the sensor.
 * @param resolution The resolution of the sensor.
 * @return sensor_config_t* A pointer to the new `sensor_config_t` instance.
 */
sensor_config_t *create_sensor_config(const char *address, int idx,
                                      int resolution);

/**
 * @brief Creates a new `bus_config_t` instance.
 *
 * @param pin The pin number for the bus.
 * @return bus_config_t* A pointer to the new `bus_config_t` instance.
 */
bus_config_t *create_bus_config(int pin);

/**
 * @brief Adds a `sensor_config_t` instance to a `bus_config_t` instance.
 *
 * @param bus A pointer to the `bus_config_t` instance.
 * @param sensor A pointer to the `sensor_config_t` instance.
 * @return void
 */
void add_sensor_config_to_bus_config(bus_config_t *bus,
                                     sensor_config_t *sensor);

/**
 * @brief Frees memory allocated for a `sensor_config_t` instance.
 *
 * @param sensor A pointer to the `sensor_config_t` instance.
 * @return void
 */
void free_sensor_config(sensor_config_t *sensor);

/**
 * @brief Frees memory allocated for a `bus_config_t` instance.
 *
 * @param bus A pointer to the `bus_config_t` instance.
 * @return void
 */
void free_bus_config(bus_config_t *bus);

/**
 * @brief Creates a new `onewire_config_t` instance.
 *
 * @return onewire_config_t* A pointer to the new `onewire_config_t` instance.
 */
onewire_config_t *create_onewire_config();

/**
 * @brief Adds a `bus_config_t` instance to a `onewire_config_t` instance.
 *
 * @param config A pointer to the `onewire_config_t` instance.
 * @param bus A pointer to the `bus_config_t` instance.
 * @return void
 */
void add_bus_to_onewire_config(onewire_config_t *config, bus_config_t *bus);

/**
 * @brief Frees memory allocated for a `onewire_config_t` instance.
 *
 * @param config A pointer to the `onewire_config_t` instance.
 * @return void
 */
void free_onewire_config(onewire_config_t *config);

/**
 * @brief Parses a 1-Wire configuration string and returns a `onewire_config_t`
 * instance.
 *
 * @param config_str The configuration string.
 * @return onewire_config_t* A pointer to the new `onewire_config_t` instance.
 */
onewire_config_t *parse_onewire_config(const char *config_str);

// mqtt_config_t functions

/**
 * @brief Parses a broker configuration string and fills a
 * `mqtt_broker_config_t` instance.
 *
 * @param connection_string The connection string.
 * @param config A pointer to the `mqtt_broker_config_t` instance.
 * @return void
 */
void parse_mqtt_broker_connection_string(const char *connection_string,
                                         mqtt_broker_config_t *config);

/**
 * @brief Parses the full MQTT connection string and fills a `mqtt_config_t`
 * instance.
 *
 * @param connection_string The connection string.
 * @param mqtt_config A pointer to the `mqtt_config_t` instance.
 * @return void
 */
void parse_mqtt_connection_string(const char *connection_string,
                                  mqtt_config_t *mqtt_config);

/**
 * @brief Frees memory allocated for a `mqtt_broker_config_t` instance.
 * @param config A pointer to the `mqtt_broker_config_t` instance.
 * @return void
 */
void free_mqtt_broker_config(mqtt_broker_config_t *config);

/**
 * @brief Frees memory allocated for a `mqtt_config_t` instance.
 * @param mqtt_config A pointer to the `mqtt_config_t` instance.
 * @return void
 */
void free_mqtt_config(mqtt_config_t *mqtt_config);

#endif // CONFIG_H

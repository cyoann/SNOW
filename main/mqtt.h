#ifndef MQTT_H
#define MQTT_H

#include "config_types.h"
#include "mqtt_client.h"

/**
 * @brief MQTT client configuration.
 *
 * This struct contains the configuration for the MQTT client.
 */
typedef struct {
  esp_mqtt_client_handle_t client;
} MQTT_Client;

/**
 * @brief Initialize the MQTT client with the given configuration.
 *
 * @param mqtt_client a pointer to the MQTT_Client struct.
 * @param mqtt_config a pointer to the mqtt_broker_config_t struct.
 * @return esp_err_t Error code indicating success or failure.
 */
esp_err_t mqtt_init(MQTT_Client *mqtt_client,
                    mqtt_broker_config_t *mqtt_config);

/**
 * @brief Start the MQTT client.
 *
 * @param mqtt_client a pointer to the MQTT_Client struct.
 */
esp_err_t mqtt_start(MQTT_Client *mqtt_client);

/**
 * @brief Publish a message to the MQTT broker.
 *
 * @param mqtt_client a pointer to the MQTT_Client struct.
 * @param config a pointer to the mqtt_broker_config_t struct.
 * @param data the message to publish.
 * @return esp_err_t Error code indicating success or failure.
 */
esp_err_t mqtt_publish(MQTT_Client *mqtt_client, mqtt_broker_config_t *config,
                       const char *data);

#endif // MQTT_H

#ifndef APP_H
#define APP_H

#include "app_types.h"
#include "config.h"
#include "mqtt.h"
#include "sensor.h"
#include "utils.h"
#include "wifi.h"

/**
 * @brief Initialize the application and its components
 *
 * This function initializes the application and its components. It should be
 * called once at the beginning of the program.
 *
 * @param state A pointer to the application state
 * @return void
 */
void app_init(app_state_t *state);

/**
 * @brief Run the application
 *
 * This function runs the application. It should be called in the main loop of
 * the program.
 *
 * @param state A pointer to the application state
 * @return void
 */
void app_run(app_state_t *state);

/**
 * @brief Deinitialize the application and its components
 *
 * This function deinitializes the application and its components. It should be
 * called once at the end of the program.
 *
 * @param state A pointer to the application state
 * @return void
 */
void app_deinit(app_state_t *state);

/**
 * @brief Appends an error to the application state
 *
 * This function appends an error to the application state.
 *
 * @param state A pointer to the application state
 * @param code The error code
 * @param message The error message
 * @return void
 */
void app_append_error(app_state_t *state, int code, const char *message);

/**
 * @brief Runs the scanner mode
 *
 * This function runs the scanner mode and it meant to be called from the
 * app_run function.
 *
 * @param state A pointer to the application state
 * @return void
 */
void run_scanner_mode(app_state_t *state);

/**
 * @brief Runs the normal mode
 *
 * This function runs the normal mode and it meant to be called from the
 * app_run function.
 *
 * @param state A pointer to the application state
 * @return void
 */
void run_normal_mode(app_state_t *state);

/**
 * @brief Initializes the 1-Wire buses
 *
 * This function initializes the 1-Wire buses.
 *
 * @param state A pointer to the application state
 * @return void
 */
void init_onewire_buses(app_state_t *state);

/**
 * @brief Calculates the number of sensors
 *
 * This function calculates the number of sensors.
 *
 * @param state A pointer to the application state
 * @return void
 */
void calculate_num_sensors(app_state_t *state);

/**
 * @brief Reads the sensors and appends the readings to the application state
 *
 * This function reads the sensors and appends the readings to the application
 * state.
 *
 * @param state A pointer to the application state
 * @return void
 */
void read_sensors(app_state_t *state);

/**
 * @brief Publishes the sensor readings to the MQTT broker
 *
 * This function publishes the sensor readings to the MQTT broker.
 *
 * @param state A pointer to the application state
 * @return void
 */
void publish_sensor_readings(app_state_t *state);

/**
 * @brief Logs errors to the console
 *
 * This function logs errors from the application state to the console.
 *
 * @param state A pointer to the application state
 * @return void
 */
void log_errors(app_state_t *state);

/**
 * @brief Logs the runtime to the console
 *
 * This function logs the runtime to the console.
 *
 * @param state A pointer to the application state
 * @return void
 */
void log_runtime(app_state_t *state);

/**
 * @brief Enters sleep mode
 *
 * This function enters sleep mode.
 *
 * @return void
 */
void enter_sleep_mode();

/**
 * @brief Frees memory allocated for the application state
 *
 * This function frees memory allocated for the application state.
 *
 * @param state A pointer to the application state
 * @return void
 */
void app_free_state(app_state_t *state);

/**
 * @brief Frees memory allocated for the errors
 *
 * This function frees memory allocated for the errors.
 *
 * @param state A pointer to the application state
 * @return void
 */
void free_errors(app_state_t *state);

#endif // APP_H

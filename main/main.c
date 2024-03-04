#include "app.h"

#include "esp_log.h"

void app_main(void) {
  app_state_t state = {
#ifdef CONFIG_ESP_DEBUG_MODE
      .start_time = {0},
#endif // CONFIG_ESP_DEBUG_MODE
      .running = true,
      .errors = NULL,
      .num_errors = 0,
      .onewire_config =
          {
              .bus_capacity = 0,
              .bus_count = 0,
              .buses = NULL,
          },
#ifndef CONFIG_ESP_SCANNER_MODE
      .sensor_readings = NULL,
      .num_sensors = 0,
#endif // CONFIG_ESP_SCANNER_MODE
  };

  app_init(&state);
  while (state.running) {
    app_run(&state);
  }
  app_deinit(&state);
}

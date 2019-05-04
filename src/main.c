/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#define LOG_TAG "main"

#include <stddef.h>

#include "service/console.h"
#include "service/monitor.h"
#include "service/python.h"

#include "global.h"
#include "log.h"
#include "service.h"

int main(int argc, char* argv[]) {
  g_mut->argc = argc;
  g_mut->argv = (const char**) argv;

  LOGI("Cozmonaut is starting up...");

  service_load(SERVICE_CONSOLE);
  service_load(SERVICE_PYTHON);
  service_load(SERVICE_MONITOR);

  service_start(SERVICE_CONSOLE);
  service_start(SERVICE_PYTHON);
  service_start(SERVICE_MONITOR);

  // Launch interactive mode
  service_call(SERVICE_PYTHON, service_python_fn_op_exec, (const void*) service_python_op_interact, NULL);

  // Open the monitor window
  service_call(SERVICE_MONITOR, service_monitor_fn_win_open, NULL, NULL);

  do {
    // Query the window status
    int window_status;
    service_call(SERVICE_MONITOR, service_monitor_fn_win_query, NULL, &window_status);

    // If window is closing, break out
    if (!window_status) {
      break;
    }

    // Update the window
    service_call(SERVICE_MONITOR, service_monitor_fn_win_update, NULL, NULL);
  } while (1);

  // Close the monitor window
  service_call(SERVICE_MONITOR, service_monitor_fn_win_close, NULL, NULL);

  service_stop(SERVICE_MONITOR);
  service_stop(SERVICE_PYTHON);
  service_stop(SERVICE_CONSOLE);

  service_unload(SERVICE_MONITOR);
  service_unload(SERVICE_PYTHON);
  service_unload(SERVICE_CONSOLE);
}

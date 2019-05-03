/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

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

  service_stop(SERVICE_MONITOR);
  service_stop(SERVICE_PYTHON);
  service_stop(SERVICE_CONSOLE);

  service_unload(SERVICE_MONITOR);
  service_unload(SERVICE_PYTHON);
  service_unload(SERVICE_CONSOLE);
}

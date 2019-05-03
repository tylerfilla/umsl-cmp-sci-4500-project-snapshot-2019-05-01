/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <stddef.h>

#include "service/console.h"
#include "global.h"
#include "log.h"
#include "service.h"

int main(int argc, char* argv[]) {
  g_mut->argc = argc;
  g_mut->argv = (const char**) argv;

  LOGI("Cozmonaut is starting up...");

  service_load(SERVICE_CONSOLE);
  service_start(SERVICE_CONSOLE);
  service_stop(SERVICE_CONSOLE);
  service_unload(SERVICE_CONSOLE);
}

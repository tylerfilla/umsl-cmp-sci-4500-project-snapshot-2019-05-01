/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <stdio.h>

#include "global.h"
#include "log.h"
#include "service.h"

int main(int argc, char* argv[]) {
  g_mut->argc = argc;
  g_mut->argv = (const char**) argv;

  printf("Hello, world!\n");

  LOGI("This is a {}!", _str("log record"));

  service_load(LOG_SERVICE);
  service_start(LOG_SERVICE);

  service_stop(LOG_SERVICE);
  service_unload(LOG_SERVICE);
}

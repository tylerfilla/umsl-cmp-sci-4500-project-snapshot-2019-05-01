/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <stddef.h>

#include "monitor.h"
#include "../service.h"

//
// Service Interface
//

static int on_load() {
  return 1; // TODO
}

static int on_unload() {
  return 1; // TODO
}

static int on_start() {
  return 1; // TODO
}

static int on_stop() {
  return 1; // TODO
}

static int (* proc(int fn))(const void* a, void* b) {
  return NULL;
}

static struct service_iface iface = {
  .on_load = &on_load,
  .on_unload = &on_unload,
  .on_start = &on_start,
  .on_stop = &on_stop,
  .proc = &proc,
};

struct service* const SERVICE_MONITOR = &(struct service) {
  .name = "monitor",
  .description = "The monitor service operates the OpenGL monitor windows.",
  .iface = &iface,
};

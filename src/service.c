/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <stdlib.h>

#include "log.h"
#include "service.h"

struct service_state {
  /** Nonzero if started, otherwise zero. */
  int started;
};

int service_load(struct service* svc) {
  LOGT("Loading {}", _str(svc->name));

  // If service has allocated state, ...
  if (svc->state) {
    // ... then the service is already loaded
    LOGE("Already loaded {}", _str(svc->name));
    return 1;
  }

  // Allocate state for service
  svc->state = calloc(1, sizeof(struct service_state));

  // Assert successful allocation
  if (!svc->state) {
    LOGE("Memory allocation failed");
    return 1;
  }

  // Call load callback
  if (svc->iface) {
    int code = svc->iface->on_load(svc);
    if (code) {
      LOGW("{} reported code {} during load", _str(svc->name), _i(code));
    }
  }

  LOGI("Loaded {}", _str(svc->name));
  LOGI("{}", _str(svc->desc));
  return 0;
}

int service_unload(struct service* svc) {
  LOGT("Unloading {}", _str(svc->name));

  // If service has no allocated state, ...
  if (!svc->state) {
    // ... then the service is not loaded
    LOGE("Not loaded {}", _str(svc->name));
    return 1;
  }

  // Call unload callback
  if (svc->iface) {
    int code = svc->iface->on_unload(svc);
    if (code) {
      LOGW("{} reported code {} during unload", _str(svc->name), _i(code));
    }
  }

  // Free service state
  free(svc->state);
  svc->state = NULL;

  LOGI("Unloaded {}", _str(svc->name));
  return 0;
}

int service_start(struct service* svc) {
  LOGT("Starting {}", _str(svc->name));

  // If service has no allocated state, ...
  if (!svc->state) {
    // ... then the service is not loaded
    LOGE("Not loaded {}", _str(svc->name));
    return 1;
  }

  // If service is already started
  if (svc->state->started) {
    LOGE("Already started {}", _str(svc->name));
    return 1;
  }

  // Call start callback
  if (svc->iface) {
    int code = svc->iface->on_start(svc);
    if (code) {
      LOGW("{} reported code {} during start", _str(svc->name), _i(code));
    }
  }

  // Set started flag
  svc->state->started = 1;

  LOGI("Started {}", _str(svc->name));
  return 0;
}

int service_stop(struct service* svc) {
  LOGT("Stopping {}", _str(svc->name));

  // If service has no allocated state, ...
  if (!svc->state) {
    // ... then the service is not loaded
    LOGE("Not loaded {}", _str(svc->name));
    return 1;
  }

  // If service is not started
  if (!svc->state->started) {
    LOGE("Not started {}", _str(svc->name));
    return 1;
  }

  // Call load callback
  if (svc->iface) {
    int code = svc->iface->on_stop(svc);
    if (code) {
      LOGW("{} reported code {} during stop", _str(svc->name), _i(code));
    }
  }

  // Clear started flag
  svc->state->started = 0;

  LOGI("Stopped {}", _str(svc->name));
  return 0;
}

int service_call(struct service* svc, int fn, const void* arg1, void* arg2) {
  // Call call callback
  if (svc->iface) {
    return svc->iface->on_call(svc, fn, arg1, arg2);
  }

  // No callback is not an error
  return 0;
}

/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <stdio.h>
#include <stdlib.h>

#include <ck_ring.h>

#include "log.h"
#include "service.h"

/** The log ring size. */
#define RING_SIZE 128

/** The log ring state. */
static ck_ring_t log__ring;

/** The log ring buffer. */
static ck_ring_buffer_t* log__ring_buffer;

//
// Service Calls
//

static int log__fn_ring_push(const void* arg1, void* arg2) {
  const struct log_request* req = arg1;

  ck_ring_enqueue_mpsc(&log__ring, &log__ring_buffer)

  return 1; // TODO
}

static int log__fn_ring_capacity(const void* arg1, void* arg2) {
  size_t* out_capacity = arg2;

  // Query log ring capacity
  *out_capacity = ck_ring_capacity(&log__ring);

  return 0;
}

static int log__fn_ring_size(const void* arg1, void* arg2) {
  size_t* out_size = arg2;

  // Query log ring size
  *out_size = ck_ring_size(&log__ring);

  return 0;
}

//
// Service Interface
//

static int on_load(struct service* svc) {
  // Allocate the log ring buffer
  log__ring_buffer = malloc(RING_SIZE * sizeof(ck_ring_buffer_t));

  // Assert allocation success
  if (!log__ring_buffer) {
    // Resort to standard error
    fprintf(stderr, "failed to allocate log ring\n");
    return 1;
  }

  // Initialize log ring
  ck_ring_init(&log__ring, RING_SIZE);

  return 0;
}

static int on_unload(struct service* svc) {
  // Free the log ring buffer
  free(log__ring_buffer);

  return 0;
}

static int on_start(struct service* svc) {
  printf("logger start\n");
  return 1; // TODO
}

static int on_stop(struct service* svc) {
  printf("logger stop\n");
  return 1; // TODO
}

static int on_call(struct service* svc, int fn, const void* arg1, void* arg2) {
  switch (fn) {
    case LOG_FN_RING_PUSH:
      return log__fn_ring_push(arg1, arg2);
    case LOG_FN_RING_CAPACITY:
      return log__fn_ring_capacity(arg1, arg2);
    case LOG_FN_RING_SIZE:
      return log__fn_ring_size(arg1, arg2);
    default:
      return 1;
  }
}

struct service_iface LOG_IFACE = {
  .on_load = &on_load,
  .on_unload = &on_unload,
  .on_start = &on_start,
  .on_stop = &on_stop,
  .on_call = &on_call,
};

//
// Service Definition
//

struct service* const LOG_SERVICE = &(struct service) {
  .name = "log",
  .desc = "The log service deals in log formatting and dispatch.",
  .iface = &LOG_IFACE,
};

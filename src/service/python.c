/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#define LOG_TAG "python"

#include <stddef.h>
#include <pthread.h>

#include <Python.h>

#include "python.h"

#include "../service.h"
#include "../log.h"

/** The Python thread. */
static pthread_t python__thread;

/** Python thread main function. */
static void* python__thread_main(void* arg) {
  LOGI("The Python thread has started");
  return NULL;
}

//
// Service Procedures
//

static int python__proc_op_exec(const void* a, void* b) {
  enum service_python_op op = (enum service_python_op) a;

  // Try to spawn the Python thread
  if (pthread_create(&python__thread, NULL, &python__thread_main, (void*) op)) {
    LOGE("Failed to spawn the Python thread");
    return 1;
  }

  return 0;
}

static int python__proc_wasd_set_fwd(const void* a, void* b) {
  return 1; // TODO
}

static int python__proc_wasd_clr_fwd(const void* a, void* b) {
  return 1; // TODO
}

static int python__proc_wasd_set_rev(const void* a, void* b) {
  return 1; // TODO
}

static int python__proc_wasd_clr_rev(const void* a, void* b) {
  return 1; // TODO
}

static int python__proc_wasd_set_left(const void* a, void* b) {
  return 1; // TODO
}

static int python__proc_wasd_clr_left(const void* a, void* b) {
  return 1; // TODO
}

static int python__proc_wasd_set_right(const void* a, void* b) {
  return 1; // TODO
}

static int python__proc_wasd_clr_right(const void* a, void* b) {
  return 1; // TODO
}

//
// Service Callbacks
//

static int on_load() {
  // Try to initialize Python interpreter
  Py_Initialize();
  if (!Py_IsInitialized()) {
    LOGE("Unable to initialize Python interpreter");
    return 1;
  }

  return 0;
}

static int on_unload() {
  // Try to clean up Python interpreter
  // It's not an error if this fails, but it might leak resources
  if (Py_FinalizeEx() < 0) {
    LOGW("Unable to clean up Python interpreter");
  }

  return 0;
}

static int on_start() {
  LOGI("The Python service has started, and library version info follows");
  LOGI("Python {}.{}.{} ({})", _i(PY_MAJOR_VERSION), _i(PY_MINOR_VERSION), _i(PY_MICRO_VERSION), _ptr(PY_VERSION_HEX));
  LOGI("Python build {}", _str(Py_GetBuildInfo()));
  LOGI("PYTHON_API_STRING={}", _str(PYTHON_API_STRING));
  LOGI("PYTHON_ABI_STRING={}", _str(PYTHON_ABI_STRING));

  return 0;
}

static int on_stop() {
  // Try to join the Python thread
  // This waits for it to die of natural causes
  // It is not an error if this call fails, but resources may leak
  if (pthread_join(python__thread, NULL)) {
    LOGW("Failed to join the Python thread");
  }

  return 0;
}

static int (* proc(int fn))(const void* a, void* b) {
  switch (fn) {
    case service_python_fn_op_exec:
      return &python__proc_op_exec;
    case service_python_fn_wasd_set_fwd:
      return &python__proc_wasd_set_fwd;
    case service_python_fn_wasd_clr_fwd:
      return &python__proc_wasd_clr_fwd;
    case service_python_fn_wasd_set_rev:
      return &python__proc_wasd_set_rev;
    case service_python_fn_wasd_clr_rev:
      return &python__proc_wasd_clr_rev;
    case service_python_fn_wasd_set_left:
      return &python__proc_wasd_set_left;
    case service_python_fn_wasd_clr_left:
      return &python__proc_wasd_clr_left;
    case service_python_fn_wasd_set_right:
      return &python__proc_wasd_set_right;
    case service_python_fn_wasd_clr_right:
      return &python__proc_wasd_clr_right;
  }

  return NULL;
}

static struct service_iface iface = {
  .on_load = &on_load,
  .on_unload = &on_unload,
  .on_start = &on_start,
  .on_stop = &on_stop,
  .proc = &proc,
};

struct service* const SERVICE_PYTHON = &(struct service) {
  .name = "python",
  .description = "The Python service hosts the Python VM, the Cozmo SDK, and our script.",
  .iface = &iface,
};

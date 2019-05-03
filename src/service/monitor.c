/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <stddef.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "monitor.h"

#include "../log.h"
#include "../service.h"

/** The initial monitor window width. */
#define WINDOW_INIT_WIDTH 1024

/** The initial monitor window height. */
#define WINDOW_INIT_HEIGHT 576

/** The monitor window title. */
#define WINDOW_TITLE "Monitor"

/** The current monitor window. */
static GLFWwindow* monitor__window;

//
// Window Callbacks
//

static void monitor__window_cb_cursor_pos(GLFWwindow* window, double xpos, double ypos) {
  LOGT("monitor__window_cb_cursor_pos({}, {}, {})", _ptr(window), _d(xpos), _d(ypos));
}

static void monitor__window_cb_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
  LOGT("monitor__window_cb_key({}, {}, {}, {}, {})", _ptr(window), _i(key), _i(scancode), _i(action), _i(mods));
}

static void monitor__window_cb_mouse_button(GLFWwindow* window, int button, int action, int mods) {
  LOGT("monitor__window_cb_mouse_button({}, {}, {}, {})", _ptr(window), _i(button), _i(action), _i(mods));
}

static void monitor__window_cb_scroll(GLFWwindow* window, double xoffset, double yoffset) {
  LOGT("monitor__window_cb_scroll({}, {}, {})", _ptr(window), _d(xoffset), _d(yoffset));
}

//
// Service Procedures
//

static int monitor__proc_win_open(const void* a, void* b) {
  if (monitor__window) {
    LOGE("Monitor window already open");
    return 1;
  }

  // Ask for OpenGL 3.2 core profile
  // This is the minimum version of GL that has profiles
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Try to create the monitor window
  monitor__window = glfwCreateWindow(WINDOW_INIT_WIDTH, WINDOW_INIT_HEIGHT, WINDOW_TITLE, NULL, NULL);
  if (!monitor__window) {
    const char* error;
    glfwGetError(&error);
    LOGE("Failed to create monitor window: {}", _str(error));
    return 1;
  }

  // Set up window callbacks
  glfwSetCursorPosCallback(monitor__window, &monitor__window_cb_cursor_pos);
  glfwSetKeyCallback(monitor__window, &monitor__window_cb_key);
  glfwSetMouseButtonCallback(monitor__window, &monitor__window_cb_mouse_button);
  glfwSetScrollCallback(monitor__window, &monitor__window_cb_scroll);

  // Make window's OpenGL context current on this thread
  glfwMakeContextCurrent(monitor__window);

  // Wait for 1 v-sync when swapping buffers
  glfwSwapInterval(1);

  int code;
  if (!(code = gladLoadGL())) {
    LOGE("Failed to load OpenGL: {}", _i(code));
    return 1;
  }

  return 0;
}

static int monitor__proc_win_close(const void* a, void* b) {
  if (!monitor__window) {
    LOGE("Monitor window not yet open");
    return 1;
  }

  // Tear down the window
  glfwDestroyWindow(monitor__window);
  monitor__window = NULL;

  return 0;
}

static int monitor__proc_win_update(const void* a, void* b) {
  if (!monitor__window) {
    LOGE("Monitor window not yet open");
    return 1;
  }

  glClearColor(rand() / (float) RAND_MAX, rand() / (float) RAND_MAX, rand() / (float) RAND_MAX, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Swap buffers on monitor window
  glfwSwapBuffers(monitor__window);

  // Poll all window events
  glfwPollEvents();

  return 0;
}

static int monitor__proc_win_query(const void* a, void* b) {
  int* out_result = b;

  if (!monitor__window) {
    LOGE("Monitor window not yet open");
    return 1;
  }

  // Test if window is closing
  if (glfwWindowShouldClose(monitor__window) == GLFW_TRUE) {
    *out_result = 0; // Window is either down or going down
  } else {
    *out_result = 1; // Window is alive and well
  }

  return 0;
}

//
// Service Callbacks
//

static int on_load() {
  // Try to initialize GLFW
  if (glfwInit() == GLFW_FALSE) {
    const char* error;
    glfwGetError(&error);
    LOGE("Failed to initialize GLFW: {}", _str(error));
    return 1;
  }

  return 0;
}

static int on_unload() {
  // Warn if a monitor window remains open
  if (monitor__window) {
    LOGW("A monitor window is still open");
  }

  // Clean up GLFW
  glfwTerminate();

  return 0;
}

static int on_start() {
  return 0;
}

static int on_stop() {
  return 0;
}

static int (* proc(int fn))(const void* a, void* b) {
  switch (fn) {
    case service_monitor_fn_win_open:
      return &monitor__proc_win_open;
    case service_monitor_fn_win_close:
      return &monitor__proc_win_close;
    case service_monitor_fn_win_update:
      return &monitor__proc_win_update;
    case service_monitor_fn_win_query:
      return &monitor__proc_win_query;
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

struct service* const SERVICE_MONITOR = &(struct service) {
  .name = "monitor",
  .description = "The monitor service operates the OpenGL monitor windows.",
  .iface = &iface,
};

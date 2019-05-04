/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#define LOG_TAG "monitor"

#include <stddef.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg/nanovg.h>
#include <nanovg/nanovg_gl.h>

#include "monitor.h"
#include "python.h"

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

/** The NanoVG context. */
static NVGcontext* monitor__nvg_context;

/** Initialize rendering. */
static void monitor__render_init() {
  LOGT("Render initialize");

  // Try to create an antialiased NanoVG context
  if (!(monitor__nvg_context = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES))) {
    LOGE("Failed to create NanoVG context");
    return;
  }
}

/** Terminate rendering. */
static void monitor__render_terminate() {
  LOGT("Render terminate");

  // Delete the NanoVG context
  nvgDeleteGL3(monitor__nvg_context);
  monitor__nvg_context = NULL;
}

/** Render a frame. */
static void monitor__render_frame() {
  // Get the latest window dimensions
  int win_width;
  int win_height;
  glfwGetWindowSize(monitor__window, &win_width, &win_height);

  // Get the latest framebuffer dimensions
  int fb_width;
  int fb_height;
  glfwGetFramebufferSize(monitor__window, &fb_width, &fb_height);

  // Compute latest device pixel ratio
  // This can change frame-to-frame if you move the window across monitors
  float dpr = fb_width / (float) win_width;

  // Define an idiomatic viewport in terms of the framebuffer dimensions
  glViewport(0, 0, fb_width, fb_height);

  // Clear frame to white
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // Begin NanoVG frame
  nvgBeginFrame(monitor__nvg_context, win_width, win_height, dpr);

  // FIXME: Do something useful
  {
    nvgShapeAntiAlias(monitor__nvg_context, 0);

    nvgBeginPath(monitor__nvg_context);
    nvgRect(monitor__nvg_context, 200, 200, 100, 100);
    nvgStrokeColor(monitor__nvg_context, nvgRGBA(0, 0, 0, 255));
    nvgStrokeWidth(monitor__nvg_context, 1);
    nvgStroke(monitor__nvg_context);

    nvgShapeAntiAlias(monitor__nvg_context, 1);

    nvgBeginPath(monitor__nvg_context);
    nvgRect(monitor__nvg_context, 350, 200, 100, 100);
    nvgStrokeColor(monitor__nvg_context, nvgRGBA(0, 0, 0, 255));
    nvgStrokeWidth(monitor__nvg_context, 1);
    nvgStroke(monitor__nvg_context);
  }

  // End NanoVG frame
  nvgEndFrame(monitor__nvg_context);
}

//
// Window Callbacks
//

static void monitor__window_cb_cursor_pos(GLFWwindow* window, double xpos, double ypos) {
  LOGT("monitor__window_cb_cursor_pos({}, {}, {})", _ptr(window), _d(xpos), _d(ypos));
}

static void monitor__window_cb_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
  switch (key) {
    case GLFW_KEY_W:
    case GLFW_KEY_UP:
      switch (action) {
        case GLFW_PRESS:
          // Set forward state
          service_call(SERVICE_PYTHON, service_python_fn_wasd_set_fwd, NULL, NULL);
          break;
        case GLFW_RELEASE:
          // Clear forward state
          service_call(SERVICE_PYTHON, service_python_fn_wasd_clr_fwd, NULL, NULL);
          break;
      }
      break;
    case GLFW_KEY_A:
    case GLFW_KEY_LEFT:
      switch (action) {
        case GLFW_PRESS:
          // Set left state
          service_call(SERVICE_PYTHON, service_python_fn_wasd_set_left, NULL, NULL);
          break;
        case GLFW_RELEASE:
          // Clear left state
          service_call(SERVICE_PYTHON, service_python_fn_wasd_clr_left, NULL, NULL);
          break;
      }
      break;
    case GLFW_KEY_S:
    case GLFW_KEY_DOWN:
      switch (action) {
        case GLFW_PRESS:
          // Set reverse state
          service_call(SERVICE_PYTHON, service_python_fn_wasd_set_rev, NULL, NULL);
          break;
        case GLFW_RELEASE:
          // Clear reverse state
          service_call(SERVICE_PYTHON, service_python_fn_wasd_clr_rev, NULL, NULL);
          break;
      }
      break;
    case GLFW_KEY_D:
    case GLFW_KEY_RIGHT:
      switch (action) {
        case GLFW_PRESS:
          // Set right state
          service_call(SERVICE_PYTHON, service_python_fn_wasd_set_right, NULL, NULL);
          break;
        case GLFW_RELEASE:
          // Clear right state
          service_call(SERVICE_PYTHON, service_python_fn_wasd_clr_right, NULL, NULL);
          break;
      }
      break;
  }
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

  // Try to load OpenGL function pointers
  int code;
  if (!(code = gladLoadGL())) {
    LOGE("Failed to load OpenGL: {}", _i(code));
    return 1;
  }

  // Initialize rendering
  monitor__render_init();

  return 0;
}

static int monitor__proc_win_close(const void* a, void* b) {
  if (!monitor__window) {
    LOGE("Monitor window not yet open");
    return 1;
  }

  // Terminate rendering
  monitor__render_terminate();

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

  // Render a frame
  monitor__render_frame();

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

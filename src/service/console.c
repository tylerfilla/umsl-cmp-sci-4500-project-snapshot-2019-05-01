/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#define LOG_TAG "console"

#include <stddef.h>

#include <ncurses.h>
#include <pthread.h>

#include "console.h"

#include "../log.h"
#include "../service.h"

/** The console thread. */
static pthread_t console__thread;

/** The console loop kill switch. */
static volatile int console__kill_switch;

static void* console__thread_main(void* arg) {
  LOGI("The console thread has started");
  return NULL;
}

//
// Service Callbacks
//

static int on_load() {
  // Initialize curses
  initscr();

  return 0;
}

static int on_unload() {
  // Tear down curses
  endwin();

  return 0;
}

static int on_start() {
  // Clear kill switch
  console__kill_switch = 0;

  // Try to spawn the console thread
  if (pthread_create(&console__thread, NULL, &console__thread_main, NULL)) {
    LOGE("Failed to spawn the console thread");
    return 1;
  }

  return 0;
}

static int on_stop() {
  // Set kill switch
  console__kill_switch = 1;

  // Try to join the console thread
  // This waits for it to die of natural causes
  if (pthread_join(console__thread, NULL)) {
    LOGE("Failed to join the console thread");
    return 1;
  }

  return 0;
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

struct service* const SERVICE_CONSOLE = &(struct service) {
  .name = "console",
  .description = "The console service runs the console user interface (CUI).",
  .iface = &iface,
};

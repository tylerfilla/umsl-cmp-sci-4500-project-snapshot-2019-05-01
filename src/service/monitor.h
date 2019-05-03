/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#ifndef SERVICE_MONITOR_H
#define SERVICE_MONITOR_H

/** Monitor service functions. */
enum service_monitor_fn {
  service_monitor_fn_win_open,
  service_monitor_fn_win_close,
  service_monitor_fn_win_update,
  service_monitor_fn_win_query,
};

/** The monitor service. */
extern struct service* const SERVICE_MONITOR;

#endif // #ifndef SERVICE_MONITOR_H

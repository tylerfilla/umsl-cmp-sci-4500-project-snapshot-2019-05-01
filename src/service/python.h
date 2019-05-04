/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#ifndef SERVICE_PYTHON_H
#define SERVICE_PYTHON_H

/** Client operations. */
enum service_python_op {
  /** Friends list mode. */
    service_python_op_friends_list,

  /** Friends remove mode. */
    service_python_op_friends_remove,

  /** Interactive mode. */
    service_python_op_interact,
};

/** Python service functions. */
enum service_python_fn {
  /** Execute a client operation. */
    service_python_fn_op_exec,

  /** Cozmo WASD control forward engage. */
    service_python_fn_wasd_set_fwd,

  /** Cozmo WASD control forward disengage. */
    service_python_fn_wasd_clr_fwd,

  /** Cozmo WASD control reverse engage. */
    service_python_fn_wasd_set_rev,

  /** Cozmo WASD control reverse disengage. */
    service_python_fn_wasd_clr_rev,

  /** Cozmo WASD control left engage. */
    service_python_fn_wasd_set_left,

  /** Cozmo WASD control left disengage. */
    service_python_fn_wasd_clr_left,

  /** Cozmo WASD control right engage. */
    service_python_fn_wasd_set_right,

  /** Cozmo WASD control right disengage. */
    service_python_fn_wasd_clr_right,
};

/** The Python service. */
extern struct service* const SERVICE_PYTHON;

#endif // #ifndef SERVICE_PYTHON_H

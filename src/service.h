/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#ifndef SERVICE_H
#define SERVICE_H

struct service_iface;
struct service_state;

/** A service definition. */
struct service {
  /** The service name. */
  const char* name;

  /** The service description. */
  const char* desc;

  /** The service interface. */
  struct service_iface* iface;

  /** The service state. */
  struct service_state* state;
};

/** A service interface. */
struct service_iface {
  /**
   * Called when the service first loads.
   *
   * @param svc The service definition
   * @return Zero on success, otherwise nonzero
   */
  int (* on_load)(struct service* svc);

  /**
   * Called when the service finally unloads.
   *
   * It will not reload.
   *
   * @param svc The service definition
   * @return Zero on success, otherwise nonzero
   */
  int (* on_unload)(struct service* svc);

  /**
   * Called when the service starts.
   *
   * @param svc The service definition
   * @return Zero on success, otherwise nonzero
   */
  int (* on_start)(struct service* svc);

  /**
   * Called when the service stops.
   *
   * It may restart.
   *
   * @param svc The service definition
   * @return Zero on success, otherwise nonzero
   */
  int (* on_stop)(struct service* svc);

  /**
   * Called when the service is issued a call.
   *
   * @param svc The service definition
   * @param fn The target function
   * @param arg1 An immutable argument
   * @param arg2 A mutable argument
   * @return Zero on success, otherwise nonzero
   */
  int (* on_call)(struct service* svc, int fn, const void* arg1, void* arg2);
};

/**
 * Load a service.
 *
 * @param svc The service definition
 * @return Zero on success, otherwise nonzero
 */
int service_load(struct service* svc);

/**
 * Unload a service.
 *
 * @param svc The service definition
 * @return Zero on success, otherwise nonzero
 */
int service_unload(struct service* svc);

/**
 * Start a service.
 *
 * @param svc The service definition
 * @return Zero on success, otherwise nonzero
 */
int service_start(struct service* svc);

/**
 * Stop a service.
 *
 * @param svc The service definition
 * @return Zero on success, otherwise nonzero
 */
int service_stop(struct service* svc);

/**
 * Call a service.
 *
 * @param svc The service definition
 * @param fn The target function
 * @param arg1 An immutable argument
 * @param arg2 A mutable argument
 * @return Zero on success, otherwise nonzero
 */
int service_call(struct service* svc, int fn, const void* arg1, void* arg2);

#endif // #ifndef SERVICE_H

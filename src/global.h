/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#ifndef GLOBAL_H
#define GLOBAL_H

/** Global process info. */
struct global {
  /** The process argument count. */
  int argc;

  /** The process argument vector. */
  const char** argv;
};

/** An immutable view of the global process info. */
extern const struct global* const g;

/** A mutable view of the global process info. */
extern struct global* const g_mut;

#endif // #ifndef GLOBAL_H

/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include "global.h"

/** The global process info. */
static struct global g__;

// The immutable view
const struct global* const g = &g__;

// The mutable view
struct global* const g_mut = &g__;

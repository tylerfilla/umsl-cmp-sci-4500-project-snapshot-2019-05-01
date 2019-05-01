/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#include <stdio.h>
#include "log.h"

void log__submit_form(struct log_form* form) {
  printf("%s [%d]\n", form->msg_fmt, form->msg_fmt_args_num);
}

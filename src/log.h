/*
 * Cozmonaut
 * Copyright 2019 The Cozmonaut Contributors
 */

#ifndef LOG_H
#define LOG_H

#include "service.h"

/** The maximum size (in bytes) of a log record. */
#define LOG_MAX_RECORD 512

/**
 * Submit a request to log. Formatting will be done on a background thread, and
 * the final formatted record will meander into the log ring after some time.
 *
 * Ordinal: 0
 *
 * Arguments:
 *   - arg1 (const struct log_request*) The log request
 *   - arg2 Not used
 *
 * Returns:
 *   - Zero on success, otherwise nonzero
 */
#define LOG_FN_SUBMIT 0

/**
 * Read up to one record from the log ring. The record is discarded even if it
 * could not be read in full, so it is advisable to be prepared to receive
 * MAX_LOG_RECORD bytes.
 *
 * Ordinal: 1
 *
 * Arguments:
 *   - arg1 (size_t) The destination buffer length
 *   - arg2 (char*) The destination buffer
 *
 * Returns
 *   - The number of bytes actually read
 */
#define LOG_FN_RING_READ 1

/**
 * Query the allocated capacity of the log ring.
 *
 * Ordinal: 2
 *
 * Arguments:
 *   - arg1 Not used
 *   - arg2 (size_t*) [out] Destination for the capacity
 *
 * Returns:
 *   - Not used
 */
#define LOG_FN_RING_CAPACITY 2

/**
 * Query the occupied size of the log ring.
 *
 * Ordinal: 3
 *
 * Arguments:
 *   - arg1 Not used
 *   - arg2 (size_t*) [out] Destination for the size
 *
 * Returns:
 *   - Not used
 */
#define LOG_FN_RING_SIZE 3

/** The log service. */
extern struct service* const LOG_SERVICE;

/** A log record severity level. */
enum log_level {
  log_level_fatal,
  log_level_error,
  log_level_warn,
  log_level_info,
  log_level_debug,
  log_level_trace,
};

/** The type of a log message format argument. */
enum log_msg_fmt_arg_type {
  log_msg_fmt_arg_type_char,
  log_msg_fmt_arg_type_signed_char,
  log_msg_fmt_arg_type_unsigned_char,
  log_msg_fmt_arg_type_short,
  log_msg_fmt_arg_type_unsigned_short,
  log_msg_fmt_arg_type_int,
  log_msg_fmt_arg_type_unsigned_int,
  log_msg_fmt_arg_type_long,
  log_msg_fmt_arg_type_unsigned_long,
  log_msg_fmt_arg_type_long_long,
  log_msg_fmt_arg_type_unsigned_long_long,
  log_msg_fmt_arg_type_float,
  log_msg_fmt_arg_type_double,
  log_msg_fmt_arg_type_long_double,
  log_msg_fmt_arg_type_string,
  log_msg_fmt_arg_type_pointer,
};

/** The value of a log message format argument. */
union log_msg_fmt_arg_value {
  char as_char;
  signed char as_signed_char;
  unsigned char as_unsigned_char;
  short as_short;
  unsigned short as_unsigned_short;
  int as_int;
  unsigned int as_unsigned_int;
  long as_long;
  unsigned long as_unsigned_long;
  long long as_long_long;
  unsigned long long as_unsigned_long_long;
  float as_float;
  double as_double;
  long double as_long_double;
  const char* as_string;
  const void* as_pointer;
};

/** A log message format argument. */
struct log_msg_fmt_arg {
  /** The argument type. */
  enum log_msg_fmt_arg_type type;

  /** The argument value. */
  union log_msg_fmt_arg_value value;
};

/** A form to fill out for submitting a log record. */
struct log_request {
  /** The log level. */
  enum log_level level;

  /** The message format string. */
  const char* msg_fmt;

  /** The message format arguments. */
  const struct log_msg_fmt_arg* msg_fmt_args;

  /** The number of message format arguments. */
  unsigned int msg_fmt_args_num;

  /** The file name. */
  const char* file;

  /** The line number. */
  unsigned int line;
};

/** @private */
#define LOG__REQUEST_MSG_ARGS(fmt, ...) \
    ((const struct log_msg_fmt_arg[]) { __VA_ARGS__ })

/** @private */
#define LOG__REQUEST_MSG_ARGS_NUM(fmt, ...)                      \
    (sizeof((const struct log_msg_fmt_arg[]) { __VA_ARGS__ }) \
      / sizeof(const struct log_msg_fmt_arg))

/**
 * Prepare a log request.
 *
 * @param lvl The log level
 * @param fmt The log message format string
 * @param ... The log message format arguments
 */
#define LOG_PREPARE(lvl, fmt, ...)                                      \
    (struct log_request) {                                              \
      .level = (enum log_level) (lvl),                                  \
      .msg_fmt = (const char*) (fmt),                                   \
      .msg_fmt_args =                                                   \
        LOG__REQUEST_MSG_ARGS((const char*) (fmt), ##__VA_ARGS__),      \
      .msg_fmt_args_num =                                               \
        LOG__REQUEST_MSG_ARGS_NUM((const char*) (fmt), ##__VA_ARGS__),  \
      .file = __FILE__,                                                 \
      .line = __LINE__,                                                 \
    }

/**
 * Submit a log record.
 *
 * @param lvl The log level
 * @param fmt The log message format string
 * @param ... The log message format arguments
 */
#define LOG(lvl, fmt, ...)                              \
    service_call(LOG_SERVICE, LOG_FN_RING_PUSH,          \
      &LOG_PREPARE((lvl), (fmt), ##__VA_ARGS__), NULL)  \

/**
 * Submit a log record with level FATAL.
 *
 * @param fmt The log message format string
 * @param ... The log message format arguments
 */
#define LOGF(fmt, ...) LOG(log_level_fatal, (fmt), ##__VA_ARGS__)

/**
 * Submit a log record with level ERROR.
 *
 * @param fmt The log message format string
 * @param ... The log message format arguments
 */
#define LOGE(fmt, ...) LOG(log_level_error, (fmt), ##__VA_ARGS__)

/**
 * Submit a log record with level WARN.
 *
 * @param fmt The log message format string
 * @param ... The log message format arguments
 */
#define LOGW(fmt, ...) LOG(log_level_warn, (fmt), ##__VA_ARGS__)

/**
 * Submit a log record with level INFO.
 *
 * @param fmt The log message format string
 * @param ... The log message format arguments
 */
#define LOGI(fmt, ...) LOG(log_level_info, (fmt), ##__VA_ARGS__)

/**
 * Submit a log record with level DEBUG.
 *
 * @param fmt The log message format string
 * @param ... The log message format arguments
 */
#define LOGD(fmt, ...) LOG(log_level_debug, (fmt), ##__VA_ARGS__)

/**
 * Submit a log record with level TRACE.
 *
 * @param fmt The log message format string
 * @param ... The log message format arguments
 */
#define LOGT(fmt, ...) LOG(log_level_trace, (fmt), ##__VA_ARGS__)

/** @private */
#define LOG__ARG_BASE(x, t1, t2)          \
    (struct log_msg_fmt_arg) {            \
      .type = log_msg_fmt_arg_type_##t1,  \
      .value.as_##t1 = (t2) (x),          \
    }

/**
 * Annotate a log format argument as type char.
 *
 * @param x The format argument
 */
#define LOG_ARG_C(x) \
    LOG__ARG_BASE((x), char, char)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type char.
 *
 * @param x The format argument
 */
#define _c(x) LOG_ARG_C(x)

#endif // #ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type signed char.
 *
 * @param x The format argument
 */
#define LOG_ARG_SC(x) \
    LOG__ARG_BASE((x), signed_char, signed char)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type signed char.
 *
 * @param x The format argument
 */
#define _sc(x) LOG_ARG_SC(x)

#endif // #ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type unsigned char.
 *
 * @param x The format argument
 */
#define LOG_ARG_UC(x) \
    LOG__ARG_BASE((x), unsigned_char, unsigned char)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type unsigned char.
 *
 * @param x The format argument
 */
#define _uc(x) LOG_ARG_UC(x)

#endif // #ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type short.
 *
 * @param x The format argument
 */
#define LOG_ARG_S(x) \
    LOG__ARG_BASE((x), short, short)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type short.
 *
 * @param x The format argument
 */
#define _s(x) LOG_ARG_S(x)

#endif // #ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type unsigned short.
 *
 * @param x The format argument
 */
#define LOG_ARG_US(x) \
    LOG__ARG_BASE((x), unsigned_short, unsigned short)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type unsigned short.
 *
 * @param x The format argument
 */
#define _us(x) LOG_ARG_US(x)

#endif // #ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type int.
 *
 * @param x The format argument
 */
#define LOG_ARG_I(x) \
    LOG__ARG_BASE((x), int, int)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type int.
 *
 * @param x The format argument
 */
#define _i(x) LOG_ARG_I(x)

#endif // #ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type unsigned int.
 *
 * @param x The format argument
 */
#define LOG_ARG_UI(x) \
    LOG__ARG_BASE((x), unsigned_int, unsigned int)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type unsigned int.
 *
 * @param x The format argument
 */
#define _ui(x) LOG_ARG_UI(x)

#endif // #ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type long.
 *
 * @param x The format argument
 */
#define LOG_ARG_L(x) \
    LOG__ARG_BASE((x), long, long)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type long.
 *
 * @param x The format argument
 */
#define _l(x) LOG_ARG_L(x)

#endif // #ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type unsigned long.
 *
 * @param x The format argument
 */
#define LOG_ARG_UL(x) \
    LOG__ARG_BASE((x), unsigned_long, unsigned long)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type unsigned long.
 *
 * @param x The format argument
 */
#define _ul(x) LOG_ARG_UL(x)

#endif // #ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type long long.
 *
 * @param x The format argument
 */
#define LOG_ARG_LL(x) \
    LOG__ARG_BASE((x), long_long, long long)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type long long.
 *
 * @param x The format argument
 */
#define _ll(x) LOG_ARG_LL(x)

#endif // #ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type unsigned long long.
 *
 * @param x The format argument
 */
#define LOG_ARG_ULL(x) \
    LOG__ARG_BASE((x), unsigned_long_long, unsigned long long)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type unsigned long long.
 *
 * @param x The format argument
 */
#define _ull(x) LOG_ARG_ULL(x)

#endif // #ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type float.
 *
 * @param x The format argument
 */
#define LOG_ARG_F(x) \
    LOG__ARG_BASE((x), float, float)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type float.
 *
 * @param x The format argument
 */
#define _f(x) LOG_ARG_F(x)

#endif // #ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type double.
 *
 * @param x The format argument
 */
#define LOG_ARG_D(x) \
    LOG__ARG_BASE((x), double, double)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type double.
 *
 * @param x The format argument
 */
#define _d(x) LOG_ARG_D(x)

#endif // #ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type long double.
 *
 * @param x The format argument
 */
#define LOG_ARG_LD(x) \
    LOG__ARG_BASE((x), long_double, long double)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type long double.
 *
 * @param x The format argument
 */
#define _ld(x) LOG_ARG_LD(x)

#endif // #ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type string.
 *
 * @param x The format argument
 */
#define LOG_ARG_STR(x) \
    LOG__ARG_BASE((x), string, const char*)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type string.
 *
 * @param x The format argument
 */
#define _str(x) LOG_ARG_STR(x)

#endif // #ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type pointer.
 *
 * @param x The format argument
 */
#define LOG_ARG_PTR(x) \
    LOG__ARG_BASE((x), pointer, const void*)

#ifndef LOG_NO_SHORT_ARGS

/**
 * Annotate a log format argument as type pointer.
 *
 * @param x The format argument
 */
#define _ptr(x) LOG_ARG_PTR(x)

#endif // #ifndef LOG_NO_SHORT_ARGS
#endif // #ifndef LOG_H

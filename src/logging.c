#include "logging.h"
#include "datetime.h"

#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

void log_info(const char *message, ...) {
  const size_t message_length = strlen(message);

  char timestamp[TIMESTAMP_LENGTH + 1];
  time_t s, ms;
  time_since_epoch(&s, &ms);
  get_timestamp(timestamp, s, ms);

  char message_with_timestamp[4 + TIMESTAMP_LENGTH + 5 + message_length + 4 +
                              1 + 1];
  sprintf(message_with_timestamp,
          "\x1b[2m"
          "[%s]"
          "\x1b[22m"
          " %s"
          "\x1b[0m"
          "\n",
          timestamp, message);

  va_list args;
  va_start(args, message);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
  vfprintf(stderr, message_with_timestamp, args);
#pragma clang diagnostic pop

  va_end(args);
}

void log_important(const char *message, ...) {
  const size_t message_length = strlen(message);

  char timestamp[TIMESTAMP_LENGTH + 1];
  time_t s, ms;
  time_since_epoch(&s, &ms);
  get_timestamp(timestamp, s, ms);

  char message_with_timestamp[5 + 4 + TIMESTAMP_LENGTH + 5 + message_length +
                              4 + 1 + 1];
  sprintf(message_with_timestamp,
          "\x1b[34m"
          "\x1b[2m"
          "[%s]"
          "\x1b[22m"
          " %s"
          "\x1b[0m"
          "\n",
          timestamp, message);

  va_list args;
  va_start(args, message);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
  vfprintf(stderr, message_with_timestamp, args);
#pragma clang diagnostic pop

  va_end(args);
}

void log_warn(const char *message, ...) {
  const size_t message_length = strlen(message);

  char timestamp[TIMESTAMP_LENGTH + 1];
  time_t s, ms;
  time_since_epoch(&s, &ms);
  get_timestamp(timestamp, s, ms);

  char message_with_timestamp[5 + 4 + TIMESTAMP_LENGTH + 5 + message_length +
                              4 + 1 + 1];
  sprintf(message_with_timestamp,
          "\x1b[33m"
          "\x1b[2m"
          "[%s]"
          "\x1b[22m"
          " %s"
          "\x1b[0m"
          "\n",
          timestamp, message);

  va_list args;
  va_start(args, message);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
  vfprintf(stderr, message_with_timestamp, args);
#pragma clang diagnostic pop

  va_end(args);
}

void log_error(const char *message, ...) {
  const size_t message_length = strlen(message);

  char timestamp[TIMESTAMP_LENGTH + 1];
  time_t s, ms;
  time_since_epoch(&s, &ms);
  get_timestamp(timestamp, s, ms);

  char message_with_timestamp[5 + 4 + TIMESTAMP_LENGTH + 5 + message_length +
                              4 + 1 + 1];
  sprintf(message_with_timestamp,
          "\x1b[31m"
          "\x1b[2m"
          "[%s]"
          "\x1b[22m"
          " %s"
          "\x1b[0m"
          "\n",
          timestamp, message);

  va_list args;
  va_start(args, message);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
  vfprintf(stderr, message_with_timestamp, args);
#pragma clang diagnostic pop

  va_end(args);
}

#include "logging.h"

#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

// Creates a new timestamp string.
// It is responsibility of the one that calls it to free the resulting
// timestamp.
static char *get_timestamp(void) {
  struct timeval now_tv;
  gettimeofday(&now_tv, NULL);

  struct tm *now = localtime(&now_tv.tv_sec);

  const size_t timestamp_length = strlen("12:59:59.123") + 1;
  char *timestamp = malloc(sizeof(char) * timestamp_length);
  strftime(timestamp, sizeof(timestamp), "%H:%M:%S", now);

  int64_t milliseconds = floor(now_tv.tv_usec / 1.0e3);
  sprintf(timestamp + strlen("12:59:59"), ".%03lld", milliseconds);

  return timestamp;
}

void log_info(const char *message, ...) {
  const size_t message_length = strlen(message);

  char *timestamp = get_timestamp();
  const size_t timestamp_length = strlen(timestamp);

  char message_with_timestamp[4 + timestamp_length + 5 + message_length + 4 +
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
  vprintf(message_with_timestamp, args);
#pragma clang diagnostic pop

  va_end(args);
  free(timestamp);
}

void log_warn(const char *message, ...) {
  const size_t message_length = strlen(message);

  char *timestamp = get_timestamp();
  const size_t timestamp_length = strlen(timestamp);

  char message_with_timestamp[5 + 4 + timestamp_length + 5 + message_length +
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
  vprintf(message_with_timestamp, args);
#pragma clang diagnostic pop

  va_end(args);
  free(timestamp);
}

void log_error(const char *message, ...) {
  const size_t message_length = strlen(message);

  char *timestamp = get_timestamp();
  const size_t timestamp_length = strlen(timestamp);

  char message_with_timestamp[5 + 4 + timestamp_length + 5 + message_length +
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
  vprintf(message_with_timestamp, args);
#pragma clang diagnostic pop

  va_end(args);
  free(timestamp);
}

#include "datetime.h"

#include <bits/time.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

void time_since_epoch(time_t *seconds, time_t *milliseconds) {
  struct timeval now_tv;
  gettimeofday(&now_tv, NULL);
  if (seconds != NULL) {
    *seconds = now_tv.tv_sec;
  }
  if (milliseconds != NULL) {
    *milliseconds = floor(now_tv.tv_usec / 1e3);
  }
}

int64_t time_since_epoch_ms(void) {
  struct timespec time;
  clock_gettime(CLOCK_MONOTONIC_RAW, &time);
  return (time.tv_sec * 1e3) + (time.tv_nsec / 1e6);
}

void get_timestamp(char *buffer, time_t seconds, time_t milliseconds) {
  struct tm *tm = localtime(&seconds);
  strftime(buffer, TIMESTAMP_LENGTH + 1, "%H:%M:%S", tm);
  sprintf(buffer + TIMESTAMP_WITHOUT_MS_LENGTH, ".%03ld", milliseconds);
}

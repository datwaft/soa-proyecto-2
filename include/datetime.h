#ifndef DATETIME_H
#define DATETIME_H

#include <time.h>

#define TIMESTAMP_LENGTH 12
#define TIMESTAMP_WITHOUT_MS_LENGTH 8

void time_since_epoch(time_t *seconds, time_t *milliseconds);

// `buffer` must be a `char` array of at least `TIMESTAMP_LENGTH + 1`
// characters.
void get_timestamp(char *buffer, time_t seconds, time_t milliseconds);

#endif // !DATETIME_H

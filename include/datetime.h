#ifndef DATETIME_H
#define DATETIME_H

#include <stdint.h>
#include <time.h>

#define TIMESTAMP_LENGTH 12
#define TIMESTAMP_WITHOUT_MS_LENGTH 8

void time_since_epoch(time_t *seconds, time_t *milliseconds);

int64_t time_since_epoch_ms(void);

// `buffer` must be a `char` array of at least `TIMESTAMP_LENGTH + 1`
// characters.
void get_timestamp(char *buffer, time_t seconds, time_t milliseconds);

#endif // !DATETIME_H

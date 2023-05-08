#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

typedef struct message_st {
  int64_t producer_id;
  struct {
    time_t seconds;
    int64_t milliseconds;
  } creation;
  int8_t random_key;
} message_t;

#define MESSAGE_INVALID                                                        \
  (message_t) {                                                                \
    .producer_id = -1, .creation = {.seconds = -1, .milliseconds = -1},        \
    .random_key = -1                                                           \
  }

#define MESSAGE_SHUTDOWN                                                       \
  (message_t) {                                                                \
    .producer_id = -2, .creation = {.seconds = -2, .milliseconds = -2},        \
    .random_key = -2                                                           \
  }

message_t message_new(int64_t producer_id);

bool message_is_valid(message_t const *message);

bool message_is_invalid(message_t const *message);

bool message_is_shutdown(message_t const *message);

// `buffer` must have a size of at least `83 + TIMESTAMP_LENGTH + 1`
void message_tostring(message_t const *message, char *buffer);

// `buffer` must have a size of at least `83 + TIMESTAMP_LENGTH + 1`
void message_tostring_no_color(message_t const *message, char *buffer);

#endif // !MESSAGE_H

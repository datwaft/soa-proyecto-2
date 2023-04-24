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

message_t message_new(int64_t producer_id);

bool message_is_valid(message_t const *message);

#endif // !MESSAGE_H

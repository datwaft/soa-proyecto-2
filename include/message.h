#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>
#include <time.h>

typedef struct message_st {
  uint64_t producer_id;
  struct {
    time_t seconds;
    int64_t milliseconds;
  } creation;
  int8_t random_key;
} message_t;

message_t message_new(uint64_t producer_id);

#endif // !MESSAGE_H

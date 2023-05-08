#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>

#include "message.h"

#define EVENT_INVALID                                                          \
  (event_t) { .kind = EVENT_KIND_invalid }

typedef struct event_st {
  enum {
    EVENT_KIND_producer_init,
    EVENT_KIND_consumer_init,
    EVENT_KIND_consume,
    EVENT_KIND_produce,
    EVENT_KIND_producer_exit,
    EVENT_KIND_consumer_exit,
    EVENT_KIND_invalid,
  } kind;
  union {
    int64_t producer_id;
    int64_t consumer_id;
  };
  message_t message;
  struct {
    time_t seconds;
    int64_t milliseconds;
  } timestamp;
} event_t;

event_t event_new_producer_init(int64_t producer_id);

event_t event_new_consumer_init(int64_t consumer_id);

event_t event_new_consume(int64_t consumer_id, message_t message);

event_t event_new_produce(int64_t producer_id, message_t message);

event_t event_new_producer_exit(int64_t producer_id);

event_t event_new_consumer_exit(int64_t consumer_id);

bool event_is_invalid(event_t const *event);

#endif // !EVENT_H

#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>

#include "message.h"

typedef struct event_st {
  enum {
    EVENT_KIND_producer_init,
    EVENT_KIND_consumer_init,
    EVENT_KIND_consume,
    EVENT_KIND_produce,
    EVENT_KIND_producer_exit,
    EVENT_KIND_consumer_exit,
  } kind;
  union {
    int64_t producer_id;
    int64_t consumer_id;
  };
  message_t message;
} event_t;

event_t event_new_producer_init(int64_t producer_id);

event_t event_new_consumer_init(int64_t consumer_id);

event_t event_new_consume(int64_t consumer_id, message_t message);

event_t event_new_produce(int64_t producer_id, message_t message);

event_t event_new_producer_exit(int64_t producer_id);

event_t event_new_consumer_exit(int64_t consumer_id);

#endif // !EVENT_H

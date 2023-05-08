#include "event.h"

#include "datetime.h"
#include "message.h"

event_t event_new_producer_init(int64_t producer_id) {
  time_t s, ms;
  time_since_epoch(&s, &ms);

  return (event_t){.kind = EVENT_KIND_producer_init,
                   .producer_id = producer_id,
                   .message = MESSAGE_INVALID,
                   .timestamp = {.seconds = s, .milliseconds = ms}};
}

event_t event_new_consumer_init(int64_t consumer_id) {
  time_t s, ms;
  time_since_epoch(&s, &ms);

  return (event_t){.kind = EVENT_KIND_consumer_init,
                   .consumer_id = consumer_id,
                   .message = MESSAGE_INVALID,
                   .timestamp = {.seconds = s, .milliseconds = ms}};
}

event_t event_new_consume(int64_t consumer_id, message_t message) {
  time_t s, ms;
  time_since_epoch(&s, &ms);

  return (event_t){.kind = EVENT_KIND_consume,
                   .consumer_id = consumer_id,
                   .message = message,
                   .timestamp = {.seconds = s, .milliseconds = ms}};
}

event_t event_new_produce(int64_t producer_id, message_t message) {
  time_t s, ms;
  time_since_epoch(&s, &ms);

  return (event_t){.kind = EVENT_KIND_produce,
                   .producer_id = producer_id,
                   .message = message,
                   .timestamp = {.seconds = s, .milliseconds = ms}};
}

event_t event_new_producer_exit(int64_t producer_id) {
  time_t s, ms;
  time_since_epoch(&s, &ms);

  return (event_t){.kind = EVENT_KIND_producer_exit,
                   .producer_id = producer_id,
                   .message = MESSAGE_INVALID,
                   .timestamp = {.seconds = s, .milliseconds = ms}};
}

event_t event_new_consumer_exit(int64_t consumer_id) {
  time_t s, ms;
  time_since_epoch(&s, &ms);

  return (event_t){.kind = EVENT_KIND_consumer_exit,
                   .consumer_id = consumer_id,
                   .message = MESSAGE_INVALID,
                   .timestamp = {.seconds = s, .milliseconds = ms}};
}

bool event_is_invalid(event_t const *event) {
  return event->kind == EVENT_KIND_invalid;
}

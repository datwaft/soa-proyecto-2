#include "event.h"

#include "datetime.h"
#include "message.h"
#include <stdio.h>

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

void event_tostring_no_color(event_t const *event, char *buffer) {
  switch (event->kind) {
  case EVENT_KIND_invalid: {
    sprintf(buffer, "Invalid event, this must not have happened.");
    break;
  }
  case EVENT_KIND_consume: {
    char timestamp[TIMESTAMP_LENGTH + 1];
    get_timestamp(timestamp, event->timestamp.seconds,
                  event->timestamp.milliseconds);
    char message[83 + TIMESTAMP_LENGTH + 1];
    message_tostring(&event->message, message);
    sprintf(buffer, "[%s] Consumer '%ld' has consumed '%s' message", timestamp,
            event->consumer_id, message);
    break;
  }
  case EVENT_KIND_produce: {
    char timestamp[TIMESTAMP_LENGTH + 1];
    get_timestamp(timestamp, event->timestamp.seconds,
                  event->timestamp.milliseconds);
    char message[83 + TIMESTAMP_LENGTH + 1];
    message_tostring(&event->message, message);
    sprintf(buffer, "[%s] Producer '%ld' has produced '%s' message", timestamp,
            event->producer_id, message);
    break;
  }
  case EVENT_KIND_consumer_init: {
    char timestamp[TIMESTAMP_LENGTH + 1];
    get_timestamp(timestamp, event->timestamp.seconds,
                  event->timestamp.milliseconds);
    sprintf(buffer, "[%s] Consumer '%ld' has been initialized", timestamp,
            event->consumer_id);
    break;
  }
  case EVENT_KIND_producer_init: {
    char timestamp[TIMESTAMP_LENGTH + 1];
    get_timestamp(timestamp, event->timestamp.seconds,
                  event->timestamp.milliseconds);
    sprintf(buffer, "[%s] Producer '%ld' has been initialized", timestamp,
            event->producer_id);
    break;
  }
  case EVENT_KIND_consumer_exit: {
    char timestamp[TIMESTAMP_LENGTH + 1];
    get_timestamp(timestamp, event->timestamp.seconds,
                  event->timestamp.milliseconds);
    sprintf(buffer, "[%s] Consumer '%ld' has been finalized", timestamp,
            event->consumer_id);
    break;
  }
  case EVENT_KIND_producer_exit: {
    char timestamp[TIMESTAMP_LENGTH + 1];
    get_timestamp(timestamp, event->timestamp.seconds,
                  event->timestamp.milliseconds);
    sprintf(buffer, "[%s] Producer '%ld' has been finalized", timestamp,
            event->producer_id);
    break;
  }
  }
}

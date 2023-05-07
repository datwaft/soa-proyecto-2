#include "event.h"

#include "message.h"

event_t event_new_producer_init(int64_t producer_id) {
  return (event_t){.kind = EVENT_KIND_producer_init,
                   .producer_id = producer_id,
                   .message = MESSAGE_INVALID};
}

event_t event_new_consumer_init(int64_t consumer_id) {
  return (event_t){.kind = EVENT_KIND_consumer_init,
                   .consumer_id = consumer_id,
                   .message = MESSAGE_INVALID};
}

event_t event_new_consume(int64_t consumer_id, message_t message) {
  return (event_t){.kind = EVENT_KIND_consume,
                   .consumer_id = consumer_id,
                   .message = message};
}

event_t event_new_produce(int64_t producer_id, message_t message) {
  return (event_t){.kind = EVENT_KIND_produce,
                   .producer_id = producer_id,
                   .message = message};
}

event_t event_new_producer_exit(int64_t producer_id) {
  return (event_t){.kind = EVENT_KIND_producer_exit,
                   .producer_id = producer_id,
                   .message = MESSAGE_INVALID};
}

event_t event_new_consumer_exit(int64_t consumer_id) {
  return (event_t){.kind = EVENT_KIND_consumer_exit,
                   .consumer_id = consumer_id,
                   .message = MESSAGE_INVALID};
}

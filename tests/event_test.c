#include <criterion/criterion.h>
#include <criterion/internal/assert.h>

#include "event.h"
#include "message.h"

// =======================
// event_new_producer_init
// =======================

Test(event_new_producer_init, standard) {
  event_t event = event_new_producer_init(99);

  cr_expect_eq(event.kind, EVENT_KIND_producer_init);
  cr_expect_eq(event.producer_id, 99);
  cr_expect(message_is_invalid(&event.message));
}

// =======================
// event_new_consumer_init
// =======================

Test(event_new_consumer_init, standard) {
  event_t event = event_new_consumer_init(99);

  cr_expect_eq(event.kind, EVENT_KIND_consumer_init);
  cr_expect_eq(event.consumer_id, 99);
  cr_expect(message_is_invalid(&event.message));
}

// =================
// event_new_consume
// =================

Test(event_new_consume, standard) {
  event_t event = event_new_consume(99, message_new(88));

  cr_expect_eq(event.kind, EVENT_KIND_consume);
  cr_expect_eq(event.consumer_id, 99);
  cr_expect(message_is_valid(&event.message));
  cr_expect_eq(event.message.producer_id, 88);
}

// =================
// event_new_produce
// =================

Test(event_new_produce, standard) {
  event_t event = event_new_produce(99, message_new(88));

  cr_expect_eq(event.kind, EVENT_KIND_produce);
  cr_expect_eq(event.producer_id, 99);
  cr_expect(message_is_valid(&event.message));
  cr_expect_eq(event.message.producer_id, 88);
}

// =======================
// event_new_producer_exit
// =======================

Test(event_new_producer_exit, standard) {
  event_t event = event_new_producer_exit(99);

  cr_expect_eq(event.kind, EVENT_KIND_producer_exit);
  cr_expect_eq(event.producer_id, 99);
  cr_expect(message_is_invalid(&event.message));
}

// =======================
// event_new_consumer_exit
// =======================

Test(event_new_consumer_exit, standard) {
  event_t event = event_new_consumer_exit(99);

  cr_expect_eq(event.kind, EVENT_KIND_consumer_exit);
  cr_expect_eq(event.consumer_id, 99);
  cr_expect(message_is_invalid(&event.message));
}

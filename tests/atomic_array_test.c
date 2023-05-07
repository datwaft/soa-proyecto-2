#include <criterion/criterion.h>

#include "atomic_array.h"

#include "event.h"

// ================
// atomic_array_new
// ================
//
Test(atomic_array_new, default) {
  atomic_array_t atomic_array = atomic_array_new();

  cr_expect_eq(atomic_array.size, 0);
}

// ========================
// atomic_array_push_unsafe
// ========================

Test(atomic_array_push_unsafe, empty) {
  atomic_array_t atomic_array = atomic_array_new();

  cr_expect(
      atomic_array_push_unsafe(&atomic_array, event_new_consumer_init(99)));
  cr_expect_eq(atomic_array.array[0].kind, EVENT_KIND_consumer_init);
  cr_expect_eq(atomic_array.array[0].consumer_id, 99);
}

Test(atomic_array_push_unsafe, full) {
  atomic_array_t atomic_array = atomic_array_new();
  for (int i = 0; i < ATOMIC_ARRAY_MAX_SIZE; ++i) {
    cr_expect(
        atomic_array_push_unsafe(&atomic_array, event_new_consumer_init(i)));
  }

  cr_expect_not(
      atomic_array_push_unsafe(&atomic_array, event_new_consumer_init(99)));
}

// =======================
// atomic_array_pop_unsafe
// =======================

Test(atomic_array_pop_unsafe, empty) {
  atomic_array_t atomic_array = atomic_array_new();

  event_t result = atomic_array_pop_unsafe(&atomic_array);

  cr_expect(event_is_invalid(&result));
  cr_expect_eq(atomic_array.size, 0);
}

Test(atomic_array_pop_unsafe, one_element) {
  atomic_array_t atomic_array = atomic_array_new();
  cr_expect(
      atomic_array_push_unsafe(&atomic_array, event_new_consumer_init(99)));

  event_t result = atomic_array_pop_unsafe(&atomic_array);

  cr_expect(!event_is_invalid(&result));
  cr_expect_eq(result.kind, EVENT_KIND_consumer_init);
  cr_expect_eq(result.consumer_id, 99);
  cr_expect_eq(atomic_array.size, 0);
}

Test(atomic_array_pop_unsafe, full) {
  atomic_array_t atomic_array = atomic_array_new();
  for (int i = 0; i < ATOMIC_ARRAY_MAX_SIZE; ++i) {
    cr_expect(
        atomic_array_push_unsafe(&atomic_array, event_new_consumer_init(i)));
  }

  event_t result = atomic_array_pop_unsafe(&atomic_array);

  cr_expect(!event_is_invalid(&result));
  cr_expect_eq(result.kind, EVENT_KIND_consumer_init);
  cr_expect_eq(result.consumer_id, ATOMIC_ARRAY_MAX_SIZE - 1);
  cr_expect_eq(atomic_array.size, ATOMIC_ARRAY_MAX_SIZE - 1);
}

// =======================
// atomic_array_get_unsafe
// =======================

Test(atomic_array_get_unsafe, valid) {
  atomic_array_t atomic_array = atomic_array_new();
  cr_expect(
      atomic_array_push_unsafe(&atomic_array, event_new_consumer_init(99)));

  event_t result = atomic_array_get_unsafe(&atomic_array, 0);

  cr_expect(!event_is_invalid(&result));
  cr_expect_eq(result.kind, EVENT_KIND_consumer_init);
  cr_expect_eq(result.consumer_id, 99);
  cr_expect_eq(atomic_array.size, 1);
}

Test(atomic_array_get_unsafe, invalid) {
  atomic_array_t atomic_array = atomic_array_new();
  cr_expect(
      atomic_array_push_unsafe(&atomic_array, event_new_consumer_init(99)));

  event_t result = atomic_array_get_unsafe(&atomic_array, 1);

  cr_expect(event_is_invalid(&result));
  cr_expect_eq(atomic_array.size, 1);
}

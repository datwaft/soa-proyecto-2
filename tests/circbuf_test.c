#include <criterion/criterion.h>
#include <stdio.h>
#include <string.h>

#include "circbuf.h"
#include "message.h"

// ===========
// circbuf_new
// ===========

Test(circbuf_new, valid) {
  circbuf_t circbuf = circbuf_new();

  cr_expect_eq(circbuf.size, 0);
  cr_expect_eq(circbuf.head, 0);
  cr_expect_eq(circbuf.tail, 0);
}

// ===========
// circbuf_get
// ===========

Test(circbuf_get, valid) {
  circbuf_t circbuf = circbuf_new();
  circbuf_push(&circbuf, message_new(99));
  circbuf_push(&circbuf, message_new(99));
  circbuf_push(&circbuf, message_new(1));
  circbuf_push(&circbuf, message_new(2));
  circbuf_push(&circbuf, message_new(3));
  circbuf_pop(&circbuf);
  circbuf_pop(&circbuf);

  cr_expect_eq(circbuf_get(&circbuf, 0).producer_id, 1);
  cr_expect_eq(circbuf_get(&circbuf, 1).producer_id, 2);
  cr_expect_eq(circbuf_get(&circbuf, 2).producer_id, 3);
}

Test(circbuf_get, invalid) {
  circbuf_t circbuf = circbuf_new();
  message_t message = circbuf_get(&circbuf, 0);

  cr_expect_not(message_is_valid(&message));
}

// ============
// circbuf_push
// ============

Test(circbuf_push, valid) {
  circbuf_t circbuf = circbuf_new();
  circbuf_push(&circbuf, message_new(1));
  circbuf_push(&circbuf, message_new(2));
  circbuf_push(&circbuf, message_new(3));

  message_t expected[CIRCBUF_MAX_SIZE] = {message_new(1), message_new(2),
                                          message_new(3)};

  cr_expect_arr_eq(circbuf.array, expected, CIRCBUF_MAX_SIZE);
  cr_expect_eq(circbuf.size, 3);
  cr_expect_eq(circbuf.head, 3);
  cr_expect_eq(circbuf.tail, 0);
}

Test(circbuf_push, invalid) {
  circbuf_t circbuf = circbuf_new();
  for (int i = 0; i < CIRCBUF_MAX_SIZE; ++i) {
    cr_expect(circbuf_push(&circbuf, message_new(i + 1)));
  }
  bool was_successful = circbuf_push(&circbuf, message_new(99));

  cr_expect_not(was_successful);
  cr_expect_eq(circbuf.size, CIRCBUF_MAX_SIZE);
  cr_expect_eq(circbuf.head, 0);
  cr_expect_eq(circbuf.tail, 0);
}

// ===========
// circbuf_pop
// ===========

Test(circbuf_pop, valid) {
  circbuf_t circbuf = circbuf_new();
  circbuf_push(&circbuf, message_new(88));
  circbuf_push(&circbuf, message_new(99));
  circbuf_push(&circbuf, message_new(1));
  circbuf_push(&circbuf, message_new(2));
  circbuf_push(&circbuf, message_new(3));
  cr_expect_eq(circbuf_pop(&circbuf).producer_id, 88);
  cr_expect_eq(circbuf_pop(&circbuf).producer_id, 99);

  cr_expect_eq(circbuf.array[2].producer_id, 1);
  cr_expect_eq(circbuf.array[3].producer_id, 2);
  cr_expect_eq(circbuf.array[4].producer_id, 3);
  cr_expect_eq(circbuf.size, 3);
  cr_expect_eq(circbuf.head, 5);
  cr_expect_eq(circbuf.tail, 2);
}

Test(circbuf_pop, invalid) {
  circbuf_t circbuf = circbuf_new();
  message_t result = circbuf_pop(&circbuf);

  cr_expect_not(message_is_valid(&result));
  cr_expect_eq(circbuf.size, 0);
  cr_expect_eq(circbuf.head, 0);
  cr_expect_eq(circbuf.tail, 0);
}

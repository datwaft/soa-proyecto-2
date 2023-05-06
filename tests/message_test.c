#include <criterion/criterion.h>

#include "message.h"

// ===========
// message_new
// ===========

Test(message_new, valid) {
  message_t message = message_new(1);
  cr_expect_eq(message.producer_id, 1);
  cr_expect_leq(message.random_key, 99);
  cr_expect_geq(message.random_key, 0);
}

// ================
// message_is_valid
// ================

Test(message_is_valid, valid) {
  message_t message = message_new(1);
  cr_expect(message_is_valid(&message));
}

Test(message_is_valid, invalid) {
  message_t message = MESSAGE_INVALID;
  cr_expect_not(message_is_valid(&message));
}

Test(message_is_valid, shutdown) {
  message_t message = MESSAGE_SHUTDOWN;
  cr_expect_not(message_is_valid(&message));
}

// ==================
// message_is_invalid
// ==================

Test(message_is_invalid, invalid) {
  message_t message = MESSAGE_INVALID;
  cr_expect(message_is_invalid(&message));
}

Test(message_is_invalid, valid) {
  message_t message = message_new(1);
  cr_expect_not(message_is_invalid(&message));
}

Test(message_is_invalid, shutdown) {
  message_t message = MESSAGE_SHUTDOWN;
  cr_expect_not(message_is_invalid(&message));
}

// ===================
// message_is_shutdown
// ===================

Test(message_is_shutdown, shutdown) {
  message_t message = MESSAGE_SHUTDOWN;
  cr_expect(message_is_shutdown(&message));
}

Test(message_is_shutdown, valid) {
  message_t message = message_new(1);
  cr_expect_not(message_is_shutdown(&message));
}

Test(message_is_shutdown, invalid) {
  message_t message = MESSAGE_INVALID;
  cr_expect_not(message_is_shutdown(&message));
}

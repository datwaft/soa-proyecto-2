#include "message.h"
#include <criterion/criterion.h>
#include <stdio.h>

Test(message_new, valid) {
  message_t message = message_new(1);
  cr_expect_eq(message.producer_id, 1);
  cr_expect_leq(message.random_key, 99);
  cr_expect_geq(message.random_key, 0);
}

Test(message_is_valid, valid) {
  message_t message = message_new(1);
  cr_expect(message_is_valid(&message));
}

Test(message_is_valid, invalid) {
  message_t message = MESSAGE_INVALID;
  cr_expect_not(message_is_valid(&message));
}

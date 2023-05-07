#include <criterion/criterion.h>

#include <time.h>

#include "datetime.h"

// =============
// get_timestamp
// =============

Test(get_timestamp, with_epoch_time) {
  time_t seconds = 1683440465;
  time_t milliseconds = 666;

  char buffer[TIMESTAMP_LENGTH + 1];
  get_timestamp(buffer, seconds, milliseconds);

  cr_assert_str_eq(buffer, "00:21:05.666");
}

Test(get_timestamp, with_raw_milliseconds) {
  time_t raw_milliseconds = 75566;

  time_t seconds = (raw_milliseconds / 1000) + 21600;
  time_t milliseconds = raw_milliseconds % 1000;

  char buffer[TIMESTAMP_LENGTH + 1];
  get_timestamp(buffer, seconds, milliseconds);

  cr_assert_str_eq(buffer, "00:01:15.566");
}

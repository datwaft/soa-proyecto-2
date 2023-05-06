#include "message.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "datetime.h"
#include "random.h"

message_t message_new(int64_t producer_id) {
  time_t s, ms;
  time_since_epoch(&s, &ms);

  return (message_t){
      .producer_id = producer_id,
      .creation = {.seconds = s, .milliseconds = ms},
      .random_key = rand_in_range(0, 99),
  };
}

bool message_is_valid(message_t const *message) {
  return (message->producer_id >= 0) && (message->creation.seconds >= 0) &&
         (message->creation.milliseconds >= 0) && (message->random_key >= 0);
}

bool message_is_invalid(message_t const *message) {
  return (message->producer_id == -1) && (message->creation.seconds == -1) &&
         (message->creation.milliseconds == -1) && (message->random_key == -1);
}

bool message_is_shutdown(message_t const *message) {
  return (message->producer_id == -2) && (message->creation.seconds == -2) &&
         (message->creation.milliseconds == -2) && (message->random_key == -2);
}

void message_tostring(message_t const *message, char *buffer) {
  char timestamp[TIMESTAMP_LENGTH + 1];
  get_timestamp(timestamp, message->creation.seconds,
                message->creation.milliseconds);
  sprintf(buffer,
          "{ producer_id: "
          "\x1b[32m"
          "%ld"
          "\x1b[39m"
          ", creation: "
          "\x1b[33m"
          "%s"
          "\x1b[39m"
          ", random_key: "
          "\x1b[34m"
          "%d"
          "\x1b[39m"
          " }",
          message->producer_id, timestamp, message->random_key);
}

#include "message.h"

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
  return (message->producer_id != -1) && (message->creation.seconds != -1) &&
         (message->creation.milliseconds != -1) && (message->random_key != -1);
}

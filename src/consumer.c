#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "atomic_integer.h"
#include "circbuf.h"
#include "datetime.h"
#include "logging.h"
#include "message.h"
#include "random.h"
#include "shared_memory.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr,
            "Usage: %s <buffer name> <delay mean in seconds>"
            "\n",
            argv[0]);
    return EXIT_FAILURE;
  }

  const char *buffer_name = argv[1];
  const double delay_mean_ms = atof(argv[2]) * 1e3;
  log_info("The mean of the delay is "
           "\x1b[3m"
           "%.0f"
           "\x1b[23m"
           "ms",
           delay_mean_ms);

  const double lambda = 1.0 / delay_mean_ms;
  log_info("λ is set to "
           "\x1b[3m"
           "%f"
           "\x1b[23m",
           lambda);

  shared_mem_t *shared_memory = get_shared_memory(buffer_name);
  if (shared_memory == (void *)IPC_FAILURE) {
    log_error("Error while obtaining shared memory: "
              "\x1b[1m"
              "%s"
              "\x1b[22m",
              strerror(errno));
    return EXIT_FAILURE;
  }

  const int64_t id = atomic_integer_add(&shared_memory->consumer_id, 1);
  log_info("Assigned "
           "\x1b[1m"
           "%d"
           "\x1b[22m"
           " as the consumer id",
           id);

  atomic_integer_add(&shared_memory->active_consumer_counter, 1);
  log_info("Increased active consumer counter");

  do {
    int sem_full_value;
    sem_getvalue(&shared_memory->full, &sem_full_value);
    if (sem_full_value <= 0) {
      log_info("Waiting for "
               "\x1b[3m"
               "full"
               "\x1b[23m"
               " sempahore...");
    }
    sem_wait(&shared_memory->full);

    size_t field = shared_memory->circbuf.tail;
    message_t message = circbuf_atomic_pop(&shared_memory->circbuf);
    if (!message_is_valid(&message)) {
      log_warn("Something went wrong, consumer tried to consume empty circular "
               "buffer");
    }
    char message_string[83 + TIMESTAMP_LENGTH + 1];
    message_tostring(&message, message_string);
    log_info("Consumed %s from ["
             "\x1b[1m"
             "%zu"
             "\x1b[22m"
             "] of the circular buffer",
             message_string, field);
    log_info("    "
             "There are "
             "\x1b[1m"
             "%ld"
             "\x1b[22m"
             " active consumers and "
             "\x1b[1m"
             "%ld"
             "\x1b[22m"
             " active producers",
             atomic_integer_get(&shared_memory->active_consumer_counter),
             atomic_integer_get(&shared_memory->active_producer_counter));

    sem_post(&shared_memory->empty);

    int64_t delay_us = rand_exp(lambda);
    log_info("Waiting "
             "\x1b[3m"
             "%ld"
             "\x1b[23m"
             "ms before consuming once again...",
             delay_us);
    usleep(delay_us * 1e3);
  } while (true);

  int64_t new_counter_value =
      atomic_integer_sub(&shared_memory->active_consumer_counter, 1);
  log_info("Decreased active consumer counter to "
           "\x1b[1;3m"
           "%ld"
           "\x1b[22;33m",
           new_counter_value);

  return EXIT_SUCCESS;
}

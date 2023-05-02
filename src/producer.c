#include <errno.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
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

  const int64_t id = atomic_integer_add(&shared_memory->producer_id, 1);
  log_info("Assigned "
           "\x1b[1m"
           "%d"
           "\x1b[22m"
           " as the producer id",
           id);

  do {
    log_info("Waiting for "
             "\x1b[3m"
             "empty"
             "\x1b[23m"
             " sempahore");
    sem_wait(&shared_memory->empty);

    message_t message = message_new(id);
    bool is_success = circbuf_atomic_push(&shared_memory->circbuf, message);
    if (!is_success) {
      log_warn("Something went wrong, producer tried to push into full "
               "circular buffer");
    }
    char message_string[83 + TIMESTAMP_LENGTH + 1];
    message_tostring(&message, message_string);
    log_info("Pushed %s into circular buffer", message_string);

    sem_post(&shared_memory->full);

    int64_t delay_us = rand_exp(lambda);
    log_info("Waiting "
             "\x1b[3m"
             "%ld"
             "\x1b[23m"
             "ms before producing once again...",
             delay_us);
    usleep(delay_us * 1e3);
  } while (true);

  return EXIT_SUCCESS;
}

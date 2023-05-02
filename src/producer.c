#include <errno.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atomic_integer.h"
#include "circbuf.h"
#include "datetime.h"
#include "logging.h"
#include "message.h"
#include "shared_memory.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr,
            "Usage: %s <buffer name>"
            "\n",
            argv[0]);
    return EXIT_FAILURE;
  }

  const char *buffer_name = argv[1];

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
  } while (true);

  return EXIT_SUCCESS;
}

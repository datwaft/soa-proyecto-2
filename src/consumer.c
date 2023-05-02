#include <errno.h>
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

  const int64_t id = atomic_integer_add(&shared_memory->consumer_id, 1);
  log_info("Assigned "
           "\x1b[1m"
           "%d"
           "\x1b[22m"
           " as the consumer id",
           id);

  do {
    sem_wait(&shared_memory->full);

    message_t message = circbuf_atomic_pop(&shared_memory->circbuf);
    if (!message_is_valid(&message)) {
      log_warn("Something went wrong, consumer tried to consume empty circular "
               "buffer");
    }
    char message_string[83 + TIMESTAMP_LENGTH + 1];
    message_tostring(&message, message_string);
    log_info("Consumed %s from circular buffer", message_string);

    sem_post(&shared_memory->empty);
  } while (true);

  return EXIT_SUCCESS;
}

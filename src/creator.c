#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atomic_integer.h"
#include "logging.h"
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

  shared_mem_t *shared_memory = create_shared_memory(buffer_name);
  if (shared_memory == (void *)IPC_FAILURE) {
    log_error("Error while creating shared memory: "
              "\x1b[1m"
              "%s",
              strerror(errno));
    return EXIT_FAILURE;
  }
  log_info("Created shared memory with buffer name "
           "\x1b[1;3m"
           "%s"
           "\x1b[22;23m",
           buffer_name);

  shared_memory->consumer_id = atomic_integer_new(0);
  log_info("Initialized consumer id counter with "
           "\x1b[1m"
           "%d"
           "\x1b[22m",
           atomic_integer_get(&shared_memory->consumer_id));

  shared_memory->producer_id = atomic_integer_new(0);
  log_info("Initialized producer id counter with "
           "\x1b[1m"
           "%d"
           "\x1b[22m",
           atomic_integer_get(&shared_memory->producer_id));

  return EXIT_SUCCESS;
}

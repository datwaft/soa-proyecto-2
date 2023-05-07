#include <errno.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atomic_array.h"
#include "atomic_boolean.h"
#include "atomic_integer.h"
#include "circbuf.h"
#include "logging.h"
#include "shared_memory.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr,
            "Usage: %s <buffer name> <circular buffer size>"
            "\n",
            argv[0]);
    return EXIT_FAILURE;
  }

  const char *buffer_name = argv[1];
  const long circbuf_max_size = atol(argv[2]);
  if (circbuf_max_size > CIRCBUF_MAX_SIZE) {
    log_error("Circular buffer size ("
              "\x1b[1m"
              "%ld"
              "\x1b[22m"
              ") argument is greater than the maximum "
              "possible ("
              "\x1b[1m"
              "%d"
              "\x1b[22m"
              ")",
              circbuf_max_size, CIRCBUF_MAX_SIZE);
    return EXIT_FAILURE;
  }

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
           "%ld"
           "\x1b[22m",
           atomic_integer_get(&shared_memory->consumer_id));

  shared_memory->producer_id = atomic_integer_new(0);
  log_info("Initialized producer id counter with "
           "\x1b[1m"
           "%ld"
           "\x1b[22m",
           atomic_integer_get(&shared_memory->producer_id));

  shared_memory->circbuf = circbuf_new(circbuf_max_size);
  log_info("Initialized circular buffer with size "
           "\x1b[1m"
           "%ld"
           "\x1b[22m",
           circbuf_max_size);

  sem_init(&shared_memory->empty, true, circbuf_max_size);
  log_info("Initialized "
           "\x1b[3m"
           "empty"
           "\x1b[23m"
           " semaphore with a value of "
           "\x1b[1m"
           "%ld"
           "\x1b[22m",
           circbuf_max_size);

  sem_init(&shared_memory->full, true, 0);
  log_info("Initialized "
           "\x1b[3m"
           "full"
           "\x1b[23m"
           " semaphore with a value of "
           "\x1b[1m"
           "%d"
           "\x1b[22m",
           0);

  shared_memory->active_consumer_counter = atomic_integer_new(0);
  log_info("Initialized active consumer counter with "
           "\x1b[1m"
           "%ld"
           "\x1b[22m",
           atomic_integer_get(&shared_memory->active_consumer_counter));

  shared_memory->active_producer_counter = atomic_integer_new(0);
  log_info("Initialized active producer counter with "
           "\x1b[1m"
           "%ld"
           "\x1b[22m",
           atomic_integer_get(&shared_memory->active_producer_counter));

  shared_memory->finished_flag = atomic_boolean_new(false);
  log_info("Initialized "
           "\x1b[3m"
           "finished"
           "\x1b[23m"
           " flag");

  shared_memory->event_history = atomic_array_new();
  log_info("Initialized event history");

  return EXIT_SUCCESS;
}

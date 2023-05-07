#include <errno.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "atomic_boolean.h"
#include "atomic_integer.h"
#include "circbuf.h"
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

  log_info("Setting finished flag to "
           "\x1b[1;3m"
           "true"
           "\x1b[22;23m");
  atomic_boolean_set(&shared_memory->finished_flag, true);

  log_info("Waiting for the active producer counter to reach 0...");
  while (atomic_integer_get(&shared_memory->active_producer_counter) > 0) {
    sem_post(&shared_memory->empty);
    sem_wait(&shared_memory->empty);
  }

  while (atomic_integer_get(&shared_memory->active_consumer_counter) > 0) {
    sem_wait(&shared_memory->empty);

    size_t field = shared_memory->circbuf.head;
    bool is_success =
        circbuf_atomic_push(&shared_memory->circbuf, MESSAGE_SHUTDOWN);
    if (!is_success) {
      log_warn("Something went wrong, finalizer tried to push into full "
               "circular buffer");
    }
    log_info("Pushed shutdown message into ["
             "\x1b[1m"
             "%zu"
             "\x1b[22m"
             "] of the circular buffer",
             field);

    sem_post(&shared_memory->full);
  }

  atomic_integer_destroy(&shared_memory->consumer_id);
  log_info("Destroyed consumer id mutex");

  atomic_integer_destroy(&shared_memory->producer_id);
  log_info("Destroyed producer id mutex");

  circbuf_destroy(&shared_memory->circbuf);
  log_info("Destroyed circular buffer mutex");

  sem_destroy(&shared_memory->empty);
  log_info("Destroyed "
           "\x1b[3m"
           "empty"
           "\x1b[23m"
           " semaphore");

  sem_destroy(&shared_memory->full);
  log_info("Destroyed "
           "\x1b[3m"
           "full"
           "\x1b[23m"
           " semaphore");

  atomic_integer_destroy(&shared_memory->active_consumer_counter);
  log_info("Destroyed active consumer counter mutex");

  atomic_integer_destroy(&shared_memory->active_producer_counter);
  log_info("Destroyed active producer counter mutex");

  log_important("Won't destroy "
                "\x1b[3m"
                "finished"
                "\x1b[23m"
                " flag mutex yet, the creator may use it");

  log_important(
      "Won't destroy event history mutex yet, the creator may use it");

  int err_free = free_shared_memory(buffer_name);
  if (err_free == IPC_FAILURE) {
    log_error("Error while freeing shared memory: "
              "\x1b[1m"
              "%s"
              "\x1b[22m",
              strerror(errno));
    return EXIT_FAILURE;
  }
  log_info("Unlinked shared memory with buffer name "
           "\x1b[1;3m"
           "%s"
           "\x1b[22;23m",
           buffer_name);

  return EXIT_SUCCESS;
}

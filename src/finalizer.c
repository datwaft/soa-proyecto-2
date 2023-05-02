#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

  int err_free = free_shared_memory(buffer_name);
  if (err_free == IPC_FAILURE) {
    log_error("Error while freeing shared memory: "
              "\x1b[1m"
              "%s"
              "\x1b[22m",
              strerror(errno));
    return EXIT_FAILURE;
  }
  log_info("Freed shared memory with buffer name "
           "\x1b[1;3m"
           "%s"
           "\x1b[22;23m",
           buffer_name);

  return EXIT_SUCCESS;
}

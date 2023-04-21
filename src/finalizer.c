#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "shared_memory.h"

extern int errno;

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
              "%s",
              strerror(errno));
    return EXIT_FAILURE;
  }
  log_info("Freed shared memory with buffer name '%s'", buffer_name);

  return EXIT_SUCCESS;
}

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

  shared_mem_t *shared_memory = get_shared_memory(buffer_name);
  if (shared_memory == (void *)IPC_FAILURE) {
    log_error("Error while obtaining shared memory: "
              "\x1b[1m"
              "%s",
              strerror(errno));
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

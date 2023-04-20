#include <stdio.h>
#include <stdlib.h>

#include "logging.h"

int main(int argc, char **argv) {
  log_info("Hello, World!");
  log_warn("Hello, World!");
  log_error("Hello, World!");
  puts("Goodbye, World!");
  return EXIT_SUCCESS;
}

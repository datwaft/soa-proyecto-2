#include <stdio.h>
#include <stdlib.h>

#include "logging.h"

int main(int argc, char *argv[]) {
  log_info("I am a producer.");

  return EXIT_SUCCESS;
}

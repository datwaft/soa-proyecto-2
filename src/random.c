#include "random.h"

#include <stdlib.h>

int64_t rand_in_range(int64_t lower_bound, int64_t upper_bound) {
  return ((rand() % (upper_bound - lower_bound + 1)) + lower_bound);
}

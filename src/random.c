#include "random.h"

#include <math.h>
#include <stdlib.h>

int64_t rand_in_range(int64_t lower_bound, int64_t upper_bound) {
  return ((rand() % (upper_bound - lower_bound + 1)) + lower_bound);
}

double rand_double(void) { return rand() / (RAND_MAX + 1.0); }

double rand_exp(double lambda) {
  // ln(1 - u)/-λ where u is an uniform random number in [0,1[ range.
  double u = rand_double();
  return log(1 - u) / -lambda;
}

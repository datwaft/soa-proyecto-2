#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

int64_t rand_in_range(int64_t lower_bound, int64_t upper_bound);

double rand_double(void);

double rand_exp(double lambda);

#endif // !RANDOM_H

#ifndef ATOMIC_INTEGER_H
#define ATOMIC_INTEGER_H

#include <semaphore.h>
#include <stdint.h>

typedef struct atomic_integer_st {
  sem_t mutex;
  int64_t value;
} atomic_integer_t;

atomic_integer_t atomic_integer_new(int64_t value);

int64_t atomic_integer_get(atomic_integer_t *atomic_integer);

int64_t atomic_integer_add(atomic_integer_t *atomic_integer, int64_t value);

int64_t atomic_integer_sub(atomic_integer_t *atomic_integer, int64_t value);

void atomic_integer_destroy(atomic_integer_t *atomic_integer);

#endif // !ATOMIC_INTEGER_H

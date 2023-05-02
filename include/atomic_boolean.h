#ifndef ATOMIC_BOOLEAN_H
#define ATOMIC_BOOLEAN_H

#include <semaphore.h>
#include <stdbool.h>

typedef struct atomic_boolean_st {
  sem_t mutex;
  bool value;
} atomic_boolean_t;

atomic_boolean_t atomic_boolean_new(bool value);

bool atomic_boolean_get(atomic_boolean_t *atomic_boolean);

bool atomic_boolean_set(atomic_boolean_t *atomic_boolean, bool value);

bool atomic_boolean_invert(atomic_boolean_t *atomic_boolean);

void atomic_boolean_destroy(atomic_boolean_t *atomic_boolean);

#endif // !ATOMIC_BOOLEAN_H

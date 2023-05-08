#ifndef ATOMIC_ARRAY_H
#define ATOMIC_ARRAY_H

#include <semaphore.h>
#include <stdbool.h>
#include <stddef.h>

#include "event.h"

#define ATOMIC_ARRAY_MAX_SIZE 100000

typedef struct atomic_array_st {
  sem_t mutex;
  event_t array[ATOMIC_ARRAY_MAX_SIZE];
  size_t size;
} atomic_array_t;

atomic_array_t atomic_array_new(void);

bool atomic_array_push(atomic_array_t *atomic_array, event_t event);

event_t atomic_array_pop(atomic_array_t *atomic_array);

event_t atomic_array_get(atomic_array_t *atomic_array, size_t pos);

bool atomic_array_push_unsafe(atomic_array_t *atomic_array, event_t event);

event_t atomic_array_pop_unsafe(atomic_array_t *atomic_array);

event_t atomic_array_get_unsafe(atomic_array_t const *atomic_array, size_t pos);

// `buffer` must have a size of at least
// `(142 + (TIMESTAMP_LENGTH * 2) + 1) * atomic_array->size + 1`
void atomic_array_tostring(atomic_array_t *atomic_array, char *buffer);

#endif // !ATOMIC_ARRAY_H

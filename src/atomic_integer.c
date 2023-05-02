#include "atomic_integer.h"

#include <semaphore.h>
#include <stdbool.h>

atomic_integer_t atomic_integer_new(int64_t value) {
  atomic_integer_t object = {.value = value};
  sem_init(&object.mutex, true, 1);
  return object;
}

int64_t atomic_integer_get(atomic_integer_t *atomic_integer) {
  sem_wait(&atomic_integer->mutex);
  int64_t result = atomic_integer->value;
  sem_post(&atomic_integer->mutex);
  return result;
}

int64_t atomic_integer_add(atomic_integer_t *atomic_integer, int64_t value) {
  sem_wait(&atomic_integer->mutex);
  atomic_integer->value += value;
  int64_t result = atomic_integer->value;
  sem_post(&atomic_integer->mutex);
  return result;
}

int64_t atomic_integer_sub(atomic_integer_t *atomic_integer, int64_t value) {
  sem_wait(&atomic_integer->mutex);
  atomic_integer->value -= value;
  int64_t result = atomic_integer->value;
  sem_post(&atomic_integer->mutex);
  return result;
}

void atomic_integer_destroy(atomic_integer_t *atomic_integer) {
  sem_destroy(&atomic_integer->mutex);
}

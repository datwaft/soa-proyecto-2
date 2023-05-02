#include "atomic_boolean.h"

atomic_boolean_t atomic_boolean_new(bool value) {
  atomic_boolean_t object = {.value = value};
  sem_init(&object.mutex, true, 1);
  return object;
}

bool atomic_boolean_get(atomic_boolean_t *atomic_boolean) {
  sem_wait(&atomic_boolean->mutex);
  bool result = atomic_boolean->value;
  sem_post(&atomic_boolean->mutex);
  return result;
}

bool atomic_boolean_set(atomic_boolean_t *atomic_boolean, bool value) {
  sem_wait(&atomic_boolean->mutex);
  atomic_boolean->value = value;
  bool result = atomic_boolean->value;
  sem_post(&atomic_boolean->mutex);
  return result;
}

bool atomic_boolean_invert(atomic_boolean_t *atomic_boolean) {
  sem_wait(&atomic_boolean->mutex);
  atomic_boolean->value = !atomic_boolean->value;
  bool result = atomic_boolean->value;
  sem_post(&atomic_boolean->mutex);
  return result;
}

void atomic_boolean_destroy(atomic_boolean_t *atomic_boolean) {
  sem_destroy(&atomic_boolean->mutex);
}

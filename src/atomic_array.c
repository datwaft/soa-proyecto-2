#include "atomic_array.h"

#include "event.h"

atomic_array_t atomic_array_new(void) {
  atomic_array_t object = {.size = 0};
  sem_init(&object.mutex, true, 1);
  return object;
}

bool atomic_array_push(atomic_array_t *atomic_array, event_t event) {
  sem_wait(&atomic_array->mutex);
  bool result = atomic_array_push_unsafe(atomic_array, event);
  sem_post(&atomic_array->mutex);
  return result;
}

event_t atomic_array_pop(atomic_array_t *atomic_array) {
  sem_wait(&atomic_array->mutex);
  event_t result = atomic_array_pop_unsafe(atomic_array);
  sem_post(&atomic_array->mutex);
  return result;
}

event_t atomic_array_get(atomic_array_t *atomic_array, size_t pos) {
  sem_wait(&atomic_array->mutex);
  event_t result = atomic_array_get_unsafe(atomic_array, pos);
  sem_post(&atomic_array->mutex);
  return result;
}

bool atomic_array_push_unsafe(atomic_array_t *atomic_array, event_t event) {
  if (atomic_array->size >= ATOMIC_ARRAY_MAX_SIZE) {
    return false;
  }
  atomic_array->array[atomic_array->size] = event;
  atomic_array->size += 1;
  return true;
}

event_t atomic_array_pop_unsafe(atomic_array_t *atomic_array) {
  if (atomic_array->size == 0) {
    return EVENT_INVALID;
  }
  atomic_array->size -= 1;
  event_t event = atomic_array->array[atomic_array->size];
  return event;
}

event_t atomic_array_get_unsafe(atomic_array_t const *atomic_array,
                                size_t pos) {
  if (pos >= atomic_array->size) {
    return EVENT_INVALID;
  }
  return atomic_array->array[pos];
}

#include "circbuf.h"
#include "datetime.h"
#include "message.h"

#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

circbuf_t circbuf_new(size_t max_size) {
  circbuf_t object = {
      .max_size = max_size > CIRCBUF_MAX_SIZE ? CIRCBUF_MAX_SIZE : max_size,
      .size = 0,
      .head = 0,
      .tail = 0,
  };
  sem_init(&object.mutex, true, 1);
  return object;
}

message_t circbuf_get(circbuf_t const *circbuf, size_t pos) {
  if (pos >= circbuf->size) {
    return CIRCBUF_NOT_FOUND;
  }
  return circbuf->array[(circbuf->tail + pos) % circbuf->max_size];
}

bool circbuf_push(circbuf_t *circbuf, message_t message) {
  if (circbuf->size >= circbuf->max_size) {
    return false;
  }
  circbuf->array[circbuf->head] = message;
  circbuf->head = (circbuf->head + 1) % circbuf->max_size;
  circbuf->size += 1;
  return true;
}

message_t circbuf_pop(circbuf_t *circbuf) {
  if (circbuf->size == 0) {
    return CIRCBUF_NOT_FOUND;
  }
  message_t message = circbuf->array[circbuf->tail];
  circbuf->tail = (circbuf->tail + 1) % circbuf->max_size;
  circbuf->size -= 1;
  return message;
}

message_t circbuf_atomic_get(circbuf_t *circbuf, size_t pos) {
  sem_wait(&circbuf->mutex);
  message_t result = circbuf_get(circbuf, pos);
  sem_post(&circbuf->mutex);
  return result;
}

bool circbuf_atomic_push(circbuf_t *circbuf, message_t message) {
  sem_wait(&circbuf->mutex);
  bool result = circbuf_push(circbuf, message);
  sem_post(&circbuf->mutex);
  return result;
}

message_t circbuf_atomic_pop(circbuf_t *circbuf) {
  sem_wait(&circbuf->mutex);
  message_t result = circbuf_pop(circbuf);
  sem_post(&circbuf->mutex);
  return result;
}

void circbuf_destroy(circbuf_t *circbuf) { sem_destroy(&circbuf->mutex); }

void circbuf_tostring(circbuf_t *circbuf, char *buffer) {
  for (size_t i = 0; i < circbuf->max_size; ++i) {
    message_t message = circbuf->array[i];
    char message_buffer[83 + TIMESTAMP_LENGTH + 1];
    message_tostring_no_color(&message, message_buffer);
    if (message_is_shutdown(&message)) {
      strcpy(message_buffer, "");
    }
    if (circbuf->head == circbuf->tail) {
      if (circbuf->size == 0) {
        strcpy(message_buffer, "");
      }
    } else if (circbuf->head < circbuf->tail) {
      if (i >= circbuf->head && i < circbuf->tail) {
        strcpy(message_buffer, "");
      }
    } else {
      if (i < circbuf->tail || i >= circbuf->head) {
        strcpy(message_buffer, "");
      }
    }

    char prefix[16] = "";
    if (i == circbuf->head && i == circbuf->tail) {
      strcpy(prefix, "Head + Tail => ");
    } else if (i == circbuf->head) {
      strcpy(prefix, "Head => ");
    } else if (i == circbuf->tail) {
      strcpy(prefix, "Tail => ");
    }

    buffer += sprintf(buffer, "%s [%zu] %s\n", prefix, i, message_buffer);
  }
}

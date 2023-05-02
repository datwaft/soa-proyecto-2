#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <semaphore.h>
#include <stdbool.h>
#include <stddef.h>

#include "message.h"

#define CIRCBUF_MAX_SIZE 16

#define CIRCBUF_NOT_FOUND MESSAGE_INVALID

typedef struct circbuf_st {
  message_t array[CIRCBUF_MAX_SIZE];
  size_t size;
  size_t head;
  size_t tail;
  sem_t mutex;
} circbuf_t;

circbuf_t circbuf_new(void);

message_t circbuf_get(circbuf_t const *circbuf, size_t pos);

bool circbuf_push(circbuf_t *circbuf, message_t message);

message_t circbuf_pop(circbuf_t *circbuf);

message_t circbuf_atomic_get(circbuf_t *circbuf, size_t pos);

bool circbuf_atomic_push(circbuf_t *circbuf, message_t message);

message_t circbuf_atomic_pop(circbuf_t *circbuf);

void circbuf_destroy(circbuf_t *circbuf);

#endif // !CIRCULAR_BUFFER_H

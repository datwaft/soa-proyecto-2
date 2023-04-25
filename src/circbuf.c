#include "circbuf.h"
#include "message.h"

circbuf_t circbuf_new(void) {
  return (circbuf_t){
      .size = 0,
      .head = 0,
      .tail = 0,
  };
}

message_t circbuf_get(circbuf_t const *circbuf, size_t pos) {
  if (pos >= circbuf->size) {
    return CIRCBUF_NOT_FOUND;
  }
  return circbuf->array[(circbuf->head + pos) % CIRCBUF_MAX_SIZE];
}

bool circbuf_push(circbuf_t *circbuf, message_t message) {
  if (circbuf->size >= CIRCBUF_MAX_SIZE) {
    return false;
  }
  circbuf->array[circbuf->tail] = message;
  circbuf->tail = (circbuf->tail + 1) % CIRCBUF_MAX_SIZE;
  circbuf->size += 1;
  return true;
}

message_t circbuf_pop(circbuf_t *circbuf) {
  if (circbuf->size == 0) {
    return CIRCBUF_NOT_FOUND;
  }
  message_t message = circbuf->array[circbuf->tail];
  circbuf->head = (circbuf->head + 1) % CIRCBUF_MAX_SIZE;
  circbuf->size -= 1;
  return message;
}

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <semaphore.h>

#include "atomic_array.h"
#include "atomic_boolean.h"
#include "atomic_integer.h"
#include "circbuf.h"

#define IPC_SUCCESS 0
#define IPC_FAILURE -1

typedef struct shared_mem_st {
  atomic_integer_t producer_id;
  atomic_integer_t consumer_id;
  circbuf_t circbuf;
  sem_t empty;
  sem_t full;
  atomic_integer_t active_producer_counter;
  atomic_integer_t active_consumer_counter;
  atomic_boolean_t finished_flag;
  atomic_array_t event_history;
} shared_mem_t;

shared_mem_t *create_shared_memory(const char *name);

shared_mem_t *get_shared_memory(const char *name);

int free_shared_memory(const char *name);

#endif // !SHARED_MEMORY_H

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#define IPC_SUCCESS 0
#define IPC_FAILURE -1

typedef struct shared_mem_st {
  char message[1 << 16];
} shared_mem_t;

shared_mem_t *create_shared_memory(const char *name);

shared_mem_t *get_shared_memory(const char *name);

int free_shared_memory(const char *name);

#endif // !SHARED_MEMORY_H

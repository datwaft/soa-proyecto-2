#include "shared_memory.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

shared_mem_t *create_shared_memory(const char *name) {
  int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    return (void *)IPC_FAILURE;
  }
  int err = ftruncate(fd, sizeof(shared_mem_t));
  if (err == -1) {
    return (void *)IPC_FAILURE;
  }
  void *shm_addr = mmap(NULL, sizeof(shared_mem_t), PROT_READ | PROT_WRITE,
                        MAP_SHARED, fd, 0);
  if (shm_addr == MAP_FAILED) {
    return (void *)IPC_FAILURE;
  }
  return shm_addr;
}

shared_mem_t *get_shared_memory(const char *name) {
  int fd = shm_open(name, O_RDWR);
  if (fd == -1) {
    return (void *)IPC_FAILURE;
  }
  void *shm_addr = mmap(NULL, sizeof(shared_mem_t), PROT_READ | PROT_WRITE,
                        MAP_SHARED, fd, 0);
  if (shm_addr == MAP_FAILED) {
    return (void *)IPC_FAILURE;
  }
  return shm_addr;
}

int free_shared_memory(const char *name) {
  int err = shm_unlink(name);
  if (err == -1)
    return IPC_FAILURE;
  return IPC_SUCCESS;
}

#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "atomic_integer.h"
#include "circbuf.h"
#include "datetime.h"
#include "event.h"
#include "logging.h"
#include "message.h"
#include "random.h"
#include "shared_memory.h"

static shared_mem_t *shared_memory;
static int64_t id;
static struct {
  int64_t wait_time_ms;
  int64_t sem_blocked_ms;
  int messages_qty;
} stats = {.wait_time_ms = 0, .sem_blocked_ms = 0, .messages_qty = 0};

static void interrupt_handler(int signal);

static void report_stats(void);

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr,
            "Usage: %s <buffer name> <delay mean in seconds>"
            "\n",
            argv[0]);
    return EXIT_FAILURE;
  }

  sigaction(SIGINT, &(struct sigaction){.sa_handler = interrupt_handler}, NULL);

  const char *buffer_name = argv[1];
  const double delay_mean_ms = atof(argv[2]) * 1e3;
  log_info("The mean of the delay is "
           "\x1b[3m"
           "%.0f"
           "\x1b[23m"
           "ms",
           delay_mean_ms);

  const double lambda = 1.0 / delay_mean_ms;
  log_info("λ is set to "
           "\x1b[3m"
           "%f"
           "\x1b[23m",
           lambda);

  shared_memory = get_shared_memory(buffer_name);
  if (shared_memory == (void *)IPC_FAILURE) {
    log_error("Error while obtaining shared memory: "
              "\x1b[1m"
              "%s"
              "\x1b[22m",
              strerror(errno));
    return EXIT_FAILURE;
  }

  id = atomic_integer_add(&shared_memory->consumer_id, 1);
  log_info("Assigned "
           "\x1b[1m"
           "%ld"
           "\x1b[22m"
           " as the consumer id",
           id);

  atomic_integer_add(&shared_memory->active_consumer_counter, 1);
  log_info("Increased active consumer counter");

  atomic_array_push(&shared_memory->event_history, event_new_consumer_init(id));

  do {
    int sem_full_value;
    sem_getvalue(&shared_memory->full, &sem_full_value);
    if (sem_full_value <= 0) {
      log_info("Waiting for "
               "\x1b[3m"
               "full"
               "\x1b[23m"
               " sempahore...");
    }

    int64_t start_ms = time_since_epoch_ms();
    sem_wait(&shared_memory->full);
    int64_t end_ms = time_since_epoch_ms();
    stats.sem_blocked_ms += (end_ms - start_ms);

    size_t field = shared_memory->circbuf.tail;
    message_t message = circbuf_atomic_pop(&shared_memory->circbuf);
    if (message_is_shutdown(&message)) {
      log_info("Consumed shutdown message from ["
               "\x1b[1m"
               "%zu"
               "\x1b[22m"
               "] of the circular buffer",
               field);
      sem_post(&shared_memory->empty);
      break;
    } else if (message_is_invalid(&message)) {
      log_warn("Something went wrong, consumer tried to consume empty circular "
               "buffer");
    }
    char message_string[83 + TIMESTAMP_LENGTH + 1];
    message_tostring(&message, message_string);
    log_info("Consumed %s from ["
             "\x1b[1m"
             "%zu"
             "\x1b[22m"
             "] of the circular buffer",
             message_string, field);
    log_info("    "
             "There are "
             "\x1b[1m"
             "%ld"
             "\x1b[22m"
             " active consumers and "
             "\x1b[1m"
             "%ld"
             "\x1b[22m"
             " active producers",
             atomic_integer_get(&shared_memory->active_consumer_counter),
             atomic_integer_get(&shared_memory->active_producer_counter));

    atomic_array_push(&shared_memory->event_history,
                      event_new_consume(id, message));

    stats.messages_qty += 1;

    pid_t pid = getpid();
    if (message.random_key == (pid % 100)) {
      log_info("The message's random key ("
               "\x1b[1m"
               "%d"
               "\x1b[22m"
               ") is equal to the PID %% 100 ("
               "\x1b[1m"
               "%d"
               "\x1b[22m"
               " %% 100 = "
               "\x1b[1m"
               "%d"
               "\x1b[22m"
               ")",
               message.random_key, pid, pid % 100);
      sem_post(&shared_memory->empty);
      break;
    }

    sem_post(&shared_memory->empty);

    int64_t delay_ms = rand_exp(lambda);
    stats.wait_time_ms += delay_ms;
    log_info("Waiting "
             "\x1b[3m"
             "%ld"
             "\x1b[23m"
             "ms before consuming once again...",
             delay_ms);
    usleep(delay_ms * 1e3);
  } while (true);

  int64_t new_counter_value =
      atomic_integer_sub(&shared_memory->active_consumer_counter, 1);
  log_info("Decreased active consumer counter to "
           "\x1b[1;3m"
           "%ld"
           "\x1b[22;33m",
           new_counter_value);

  atomic_array_push(&shared_memory->event_history, event_new_consumer_exit(id));

  report_stats();

  return EXIT_SUCCESS;
}

static void interrupt_handler(int signal) {
  int64_t new_counter_value =
      atomic_integer_sub(&shared_memory->active_consumer_counter, 1);
  fprintf(stderr, "\n");
  log_info("Decreased active consumer counter to "
           "\x1b[1;3m"
           "%ld"
           "\x1b[22;33m",
           new_counter_value);

  atomic_array_push(&shared_memory->event_history, event_new_consumer_exit(id));

  report_stats();

  exit(EXIT_SUCCESS);
}

static void report_stats(void) {
  time_t wait_time_sec = (stats.wait_time_ms / 1000) + 21600;
  time_t wait_time_ms = stats.wait_time_ms % 1000;
  char wait_time_timestamp[TIMESTAMP_LENGTH + 1];
  get_timestamp(wait_time_timestamp, wait_time_sec, wait_time_ms);

  time_t sema_time_sec = (stats.sem_blocked_ms / 1000) + 21600;
  time_t sema_time_ms = stats.sem_blocked_ms % 1000;
  char sema_time_timestamp[TIMESTAMP_LENGTH + 1];
  get_timestamp(sema_time_timestamp, sema_time_sec, sema_time_ms);

  fprintf(stderr,
          "\n"
          "\x1b[1m"
          "CONSUMER - STATISTICS"
          "\x1b[22m"
          "\n"
          "\x1b[3m"
          "Consumer identification: "
          "\x1b[23m"
          "\x1b[1;32m"
          "%ld"
          "\x1b[22;39m"
          "\n"
          "\x1b[3m"
          "Messages consumed: "
          "\x1b[23m"
          "\x1b[1;33m"
          "%d"
          "\x1b[22;39m"
          "\n"
          "\x1b[3m"
          "Time consumed waiting to access the buffer: "
          "\x1b[23m"
          "\x1b[1;33m"
          "%s"
          "\x1b[22m"
          " ("
          "\x1b[1m"
          "%ld"
          "\x1b[22m"
          "ms)"
          "\x1b[39m"
          "\n"
          "\x1b[3m"
          "Time consumed locked by semaphores: "
          "\x1b[23m"
          "\x1b[1;33m"
          "%s"
          "\x1b[22m"
          " ("
          "\x1b[1m"
          "%ld"
          "\x1b[22m"
          "ms)"
          "\x1b[39m"
          "\n",
          id, stats.messages_qty, wait_time_timestamp, stats.wait_time_ms,
          sema_time_timestamp, stats.sem_blocked_ms);
}

#include <errno.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "atomic_boolean.h"
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

  id = atomic_integer_add(&shared_memory->producer_id, 1);
  log_info("Assigned "
           "\x1b[1m"
           "%ld"
           "\x1b[22m"
           " as the producer id",
           id);

  atomic_integer_add(&shared_memory->active_producer_counter, 1);
  log_info("Increased active producer counter");

  atomic_array_push(&shared_memory->event_history, event_new_producer_init(id));

  do {
    int sem_empty_value;
    sem_getvalue(&shared_memory->empty, &sem_empty_value);
    if (sem_empty_value <= 0) {
      log_info("Waiting for "
               "\x1b[3m"
               "empty"
               "\x1b[23m"
               " sempahore...");
    }

    int64_t start_ms = time_since_epoch_ms();
    sem_wait(&shared_memory->empty);
    int64_t end_ms = time_since_epoch_ms();
    stats.sem_blocked_ms += (end_ms - start_ms);

    if (atomic_boolean_get(&shared_memory->finished_flag)) {
      log_info("Detected "
               "\x1b[3m"
               "finished"
               "\x1b[23m"
               " flag set to "
               "\x1b[1;3m"
               "true"
               "\x1b[22;23m");
      sem_post(&shared_memory->empty);

      break;
    }

    message_t message = message_new(id);
    size_t field = shared_memory->circbuf.head;
    bool is_success = circbuf_atomic_push(&shared_memory->circbuf, message);
    if (!is_success) {
      log_warn("Something went wrong, producer tried to push into full "
               "circular buffer");
    }
    char message_string[83 + TIMESTAMP_LENGTH + 1];
    message_tostring(&message, message_string);
    log_info("Pushed %s into ["
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
                      event_new_produce(id, message));

    stats.messages_qty += 1;

    sem_post(&shared_memory->full);

    int64_t delay_ms = rand_exp(lambda);
    stats.wait_time_ms += delay_ms;
    log_info("Waiting "
             "\x1b[3m"
             "%ld"
             "\x1b[23m"
             "ms before producing once again...",
             delay_ms);
    usleep(delay_ms * 1e3);
  } while (true);

  int64_t new_counter_value =
      atomic_integer_sub(&shared_memory->active_producer_counter, 1);
  log_info("Decreased active producer counter to "
           "\x1b[1;3m"
           "%ld"
           "\x1b[22;33m",
           new_counter_value);

  atomic_array_push(&shared_memory->event_history, event_new_producer_exit(id));

  report_stats();

  return EXIT_SUCCESS;
}

void interrupt_handler(int signal) {
  int64_t new_counter_value =
      atomic_integer_sub(&shared_memory->active_producer_counter, 1);
  fprintf(stderr, "\n");
  log_info("Decreased active producer counter to "
           "\x1b[1;3m"
           "%ld"
           "\x1b[22;33m",
           new_counter_value);

  atomic_array_push(&shared_memory->event_history, event_new_producer_exit(id));

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
          "PRODUCER - STATISTICS"
          "\x1b[22m"
          "\n"
          "\x1b[3m"
          "Producer identification: "
          "\x1b[23m"
          "\x1b[1;32m"
          "%ld"
          "\x1b[22;39m"
          "\n"
          "\x1b[3m"
          "Messages produced: "
          "\x1b[23m"
          "\x1b[1;33m"
          "%d"
          "\x1b[22;39m"
          "\n"
          "\x1b[3m"
          "Time consumed waiting: "
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

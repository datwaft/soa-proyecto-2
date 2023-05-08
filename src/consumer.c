#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> //used to calculate sem waiting
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

static void interrupt_handler(int signal);

typedef struct consumer_stats_st {
  int64_t consumer_id;
  int64_t wait_time_ms;
  int64_t sem_blocked_ms;
  int messages_qty;
} consumer_stats_t;

consumer_stats_t consumer_stats = {.consumer_id = 0,
                                   .wait_time_ms = 0,
                                   .sem_blocked_ms = 0,
                                   .messages_qty = 0};

time_t start_t, end_t;
double diff_t;

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

  consumer_stats.consumer_id = id; // saving consumer id number

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
    time(&start_t); // getting start time blocked for semaphore

    sem_wait(&shared_memory->full);

    time(&end_t); // getting the end time blocked by semaphore
    diff_t = difftime(end_t, start_t);
    consumer_stats.sem_blocked_ms +=
        (int64_t)(diff_t * 1000); // saving time in ms blocked by sem.

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

    consumer_stats.messages_qty += 1; // saving quantity of messages

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

    consumer_stats.wait_time_ms += delay_ms; // saving waiting time

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

  // Showing statistics
  time_t wait_time_sec = (consumer_stats.wait_time_ms / 1000) + 21600;
  time_t wait_time_ms = consumer_stats.wait_time_ms % 1000;
  time_t sema_time_sec = (consumer_stats.sem_blocked_ms / 1000) + 21600;
  time_t sema_time_ms = consumer_stats.sem_blocked_ms % 1000;
  char wait_hour_format[TIMESTAMP_LENGTH + 1];
  char sema_hour_format[TIMESTAMP_LENGTH + 1];
  get_timestamp(wait_hour_format, wait_time_sec, wait_time_ms);
  get_timestamp(sema_hour_format, sema_time_sec, sema_time_ms);
  log_info("\x1b[1;39m" // Bold (1;) & White (39m)
           "\n \n CONSUMER - FINAL EXECUTION: STATISTICS"
           "\x1b[22;39m" // Normal & white
           "\x1b[3;39m"  // Italic & white
           "\n Consumer identification: "
           "\x1b[1;32m" // Bold ( 1; ) & Green ( 32m )
           "%ld"
           "\x1b[22;39m"
           "\n Messages pulled from the Buffer: "
           "\x1b[1;33m"
           "%d"
           "\x1b[22;39m"
           "\n Time consumed waiting to access the buffer: "
           "\x1b[1;33m"
           "%s "
           "[%ld ms]"
           "\x1b[22;39m"
           "\n Time consumed locked by semaphores: "
           "\x1b[1;33m"
           "%s "
           "[%ld ms]"
           "\x1b[22;39m"
           "\n",
           consumer_stats.consumer_id, consumer_stats.messages_qty,
           wait_hour_format, consumer_stats.wait_time_ms, sema_hour_format,
           consumer_stats.sem_blocked_ms);

  return EXIT_SUCCESS;
}

void interrupt_handler(int signal) {
  int64_t new_counter_value =
      atomic_integer_sub(&shared_memory->active_consumer_counter, 1);
  fprintf(stderr, "\n");
  log_info("Decreased active consumer counter to "
           "\x1b[1;3m"
           "%ld"
           "\x1b[22;33m",
           new_counter_value);

  atomic_array_push(&shared_memory->event_history, event_new_consumer_exit(id));

  exit(EXIT_SUCCESS);
}

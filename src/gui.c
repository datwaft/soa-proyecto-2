#include "gui.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "atomic_array.h"
#include "circbuf.h"
#include "datetime.h"
#include "logging.h"
#include "message.h"
#include "shared_memory.h"

static char const *g_buffer_name;
static bool finished = false;

int run_gui(char const *buffer_name) {
  g_buffer_name = buffer_name;
  return g_application_run(G_APPLICATION(application_new()), 0, 0);
}

GtkApplication *application_new(void) {
  GtkApplication *application =
      gtk_application_new(APPLICATION_ID, G_APPLICATION_FLAGS_NONE);
  g_signal_connect(application, "activate", G_CALLBACK(application_on_activate),
                   NULL);
  return application;
}

void application_on_activate(GtkApplication *app) {
  user_data_t user_data = {
      .builder = gtk_builder_new_from_resource(TEMPLATE_URI),
  };
  gtk_builder_connect_signals(user_data.builder, &user_data);

  GtkLabel *lbl_buffer_name =
      GTK_LABEL(gtk_builder_get_object(user_data.builder, "lbl_buffer_name"));
  gtk_label_set_text(lbl_buffer_name, g_buffer_name);

  GtkWidget *window =
      GTK_WIDGET(gtk_builder_get_object(user_data.builder, "window_main"));
  gtk_widget_show_all(window);

  shared_mem_t *shared_memory = get_shared_memory(g_buffer_name);

  while (true) {
    while (gtk_events_pending()) {
      gtk_main_iteration();
    }
    if (finished) {
      break;
    }

    char consumer_counter_str[20];
    sprintf(consumer_counter_str, "%ld",
            shared_memory->active_consumer_counter.value);

    GtkLabel *lbl_num_consumers = GTK_LABEL(
        gtk_builder_get_object(user_data.builder, "lbl_num_consumers"));
    gtk_label_set_text(lbl_num_consumers, consumer_counter_str);

    char producer_counter_str[20];
    sprintf(producer_counter_str, "%ld",
            shared_memory->active_producer_counter.value);

    GtkLabel *lbl_num_producers = GTK_LABEL(
        gtk_builder_get_object(user_data.builder, "lbl_num_producers"));
    gtk_label_set_text(lbl_num_producers, producer_counter_str);

    update_circbuf_content(shared_memory, &user_data);
    update_event_history_content(shared_memory, &user_data);
  }
}

void update_circbuf_content(shared_mem_t *shared_memory,
                            user_data_t *user_data) {
  GtkTextBuffer *circbuf_text_buffer = GTK_TEXT_BUFFER(
      gtk_builder_get_object(user_data->builder, "txt_buff_circular_buffer"));

  char
      buffer[(83 + TIMESTAMP_LENGTH + 1) * shared_memory->circbuf.max_size + 1];
  circbuf_tostring(&shared_memory->circbuf, buffer);
  gtk_text_buffer_set_text(circbuf_text_buffer, buffer, -1);
}

void update_event_history_content(shared_mem_t *shared_memory,
                                  user_data_t *user_data) {
  GtkTextBuffer *event_history_text_buffer = GTK_TEXT_BUFFER(
      gtk_builder_get_object(user_data->builder, "txt_buffer_events"));

  char buffer[(142 + (TIMESTAMP_LENGTH * 2) + 1) *
                  shared_memory->event_history.size +
              1];
  atomic_array_tostring(&shared_memory->event_history, buffer);
  gtk_text_buffer_set_text(event_history_text_buffer, buffer, -1);
}

void window_on_delete_event(GtkWidget *widget) { finished = true; }

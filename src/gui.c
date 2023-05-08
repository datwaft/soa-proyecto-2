#include "gui.h"

#include "circbuf.h"
#include "datetime.h"
#include "logging.h"
#include "shared_memory.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *buffer_name_g;

void print_all_buffer_contents(GtkTextBuffer *buffer,
                               shared_mem_t *shared_memory);

int launch_gui_log(char *buffer_name) {
  buffer_name_g = buffer_name;
  return g_application_run(G_APPLICATION(application_new()), 0, 0);
}

GtkApplication *application_new(void) {
  GtkApplication *application =
      gtk_application_new(APPLICATION_ID, G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(application, "activate", G_CALLBACK(application_on_activate),
                   NULL);
  return application;
}

void application_on_activate(GtkApplication *app, gpointer _) {

  user_data_t user_data = {
      .builder = gtk_builder_new_from_resource(TEMPLATE_URI),
  };

  GtkTextBuffer *buffer = GTK_TEXT_BUFFER(
      gtk_builder_get_object(user_data.builder, "txt_buff_circular_buffer"));
  // GtkTextIter end;
  // gtk_text_buffer_get_end_iter(buffer, &end);

  // ///

  // for (int i = 0; i < 50; i++) {
  //   char str[20];
  //   sprintf(str, "%d  %s", i, "hello\n");
  //   gtk_text_buffer_insert(buffer, &end, str, -1);
  // }

  ///

  GtkLabel *lbl_buffer_name =
      GTK_LABEL(gtk_builder_get_object(user_data.builder, "lbl_buffer_name"));
  gtk_label_set_text(lbl_buffer_name, buffer_name_g);

  GtkLabel *lbl_num_consumers =
      GTK_LABEL(gtk_builder_get_object(user_data.builder, "lbl_num_consumers"));

  GtkLabel *lbl_num_producers =
      GTK_LABEL(gtk_builder_get_object(user_data.builder, "lbl_num_producers"));

  GtkWidget *window =
      GTK_WIDGET(gtk_builder_get_object(user_data.builder, "window_main"));
  gtk_widget_show_all(window);

  // gtk_main();

  char consumer_counter_str[20];
  char producer_counter_str[20];
  // char msg_id[53];
  shared_mem_t *shared_memory = get_shared_memory(buffer_name_g);
  while (1) {
    while (gtk_events_pending())
      gtk_main_iteration();

    atomic_integer_t consumer_counter = shared_memory->active_consumer_counter;
    atomic_integer_t producer_counter = shared_memory->active_producer_counter;

    sprintf(consumer_counter_str, "%d", consumer_counter.value);
    sprintf(producer_counter_str, "%d", producer_counter.value);

    gtk_label_set_text(lbl_num_consumers, consumer_counter_str);
    gtk_label_set_text(lbl_num_producers, producer_counter_str);

    print_all_buffer_contents(buffer, shared_memory);
  }
}

void print_all_buffer_contents(GtkTextBuffer *buffer,
                               shared_mem_t *shared_memory) {

  circbuf_t *circ_buf = &shared_memory->circbuf;
  size_t size = circ_buf->max_size;
  char msg_id[53];

  GtkTextIter end;
  gtk_text_buffer_get_end_iter(buffer, &end);

  char timestamp[TIMESTAMP_LENGTH + 1];
  time_t s, ms;
  time_since_epoch(&s, &ms);
  get_timestamp(timestamp, s, ms);

  char message_with_timestamp[4 + TIMESTAMP_LENGTH + 5 + 0 + 4 + 1 + 1];
  sprintf(message_with_timestamp, " [ %s ] %s \n", timestamp,
          "==================================");

  gtk_text_buffer_insert(buffer, &end, message_with_timestamp, -1);

  for (size_t i = 0; i < size; i++) {
    message_t msg = circ_buf->array[i];

    if (msg.producer_id > 0) {
      sprintf(msg_id,
              "Buffer Position: %ld | Producer Id: %ld | Random Key: %d\n", i,
              msg.producer_id, msg.random_key);
      // g_print(msg_id);
      gtk_text_buffer_insert(buffer, &end, msg_id, -1);
    } else {
      sprintf(msg_id,
              "Buffer Position: %ld | Producer Id: %s | Random Key: %s\n", i,
              "-", "-");
      // g_print(msg_id);
      gtk_text_buffer_insert(buffer, &end, msg_id, -1);
    }
  }
  // sleep(1);
  // g_print("------------------------------------------\n");
  gtk_text_buffer_insert(
      buffer, &end, "===============================================\n", -1);
}

void window_on_delete_event(GtkWidget *widget, gpointer user_data) {
  gtk_main_quit();
}

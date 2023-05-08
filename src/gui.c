#include "gui.h"

#include "atomic_array.h"
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

int launch_gui_log(char *buffer_name) {
  buffer_name_g = buffer_name;
  return g_application_run(G_APPLICATION(application_new()), 0, 0);
}

GtkApplication *application_new(void) {
  GtkApplication *application =
      gtk_application_new(APPLICATION_ID, G_APPLICATION_FLAGS_NONE);
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

  GtkTextBuffer *txt_buffer_events = GTK_TEXT_BUFFER(
      gtk_builder_get_object(user_data.builder, "txt_buffer_events"));
  GtkTextIter start;
  gtk_text_buffer_get_start_iter(buffer, &start);
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
  // char buffer_content[53];
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

    update_all_buffer_contents(buffer, shared_memory);
    update_events(user_data, shared_memory);
  }
}

void update_all_buffer_contents(GtkTextBuffer *buffer,
                                shared_mem_t *shared_memory) {

  circbuf_t *circ_buf = &shared_memory->circbuf;
  size_t size = circ_buf->max_size;
  size_t head = circ_buf->head;
  size_t tail = circ_buf->tail;

  char buffer_content[53];
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

  char head_tail_str[50];
  sprintf(head_tail_str, "Head Position: %ld, Tail Position: %ld\n", head,
          tail);
  gtk_text_buffer_insert(buffer, &end, head_tail_str, -1);

  for (size_t i = 0; i < size; i++) {
    message_t msg = circ_buf->array[i];

    if (msg.producer_id > 0) {
      sprintf(buffer_content,
              "Buffer Position: %ld | Producer Id: %ld | Random Key: %d\n", i,
              msg.producer_id, msg.random_key);
      // g_print(buffer_content);
      gtk_text_buffer_insert(buffer, &end, buffer_content, -1);
    } else {
      sprintf(buffer_content, "Buffer: empty\n");

      // g_print(buffer_content);
      gtk_text_buffer_insert(buffer, &end, buffer_content, -1);
      break;
    }
  }

  // g_print("------------------------------------------\n");
  gtk_text_buffer_insert(
      buffer, &end, "===============================================\n", -1);
}

// void refresh_text_view(user_data_t user_data, char *text) {
//   // char text[100] = "hello world";
//   GtkWidget *txt_buffer = gtk_text_buffer_new(NULL);
//   GtkTextIter end;
//   gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(txt_buffer), &end);
//   gtk_text_buffer_insert(GTK_TEXT_BUFFER(txt_buffer), &end, text, -1);

//   GtkTextView *tv = GTK_TEXT_VIEW(
//       gtk_builder_get_object(user_data.builder, "txt_view_event"));
//   gtk_text_view_set_buffer(tv, GTK_TEXT_BUFFER(txt_buffer));
// }

void update_events(user_data_t user_data, shared_mem_t *shared_memory) {
  char str[sizeof(event_t)];

  GtkWidget *txt_buffer = gtk_text_buffer_new(NULL);
  GtkTextIter end;
  gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(txt_buffer), &end);

  for (int i = 0; i < ATOMIC_ARRAY_MAX_SIZE; i++) {
    event_t events = atomic_array_get(&shared_memory->event_history, i);
    if (events.kind == 6)
      break;

    sprintf(str, "%d %ld, %ld\n", events.kind, events.producer_id,
            events.consumer_id);
    gtk_text_buffer_insert(GTK_TEXT_BUFFER(txt_buffer), &end, str, -1);
  }

  GtkTextView *tv = GTK_TEXT_VIEW(
      gtk_builder_get_object(user_data.builder, "txt_view_event"));
  gtk_text_view_set_buffer(tv, GTK_TEXT_BUFFER(txt_buffer));
}

void window_on_delete_event(GtkWidget *widget, gpointer user_data) {
  gtk_main_quit();
}

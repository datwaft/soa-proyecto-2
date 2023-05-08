#ifndef GUI_H
#define GUI_H

#include "shared_memory.h"
#include <gtk/gtk.h>

// Application constants
#define APPLICATION_ID "com.soa.pr02"
#define TEMPLATE_URI "/com/soa/pr02/template.ui"

typedef struct user_data_st {
  GtkBuilder *builder;
} user_data_t;

GtkApplication *application_new(void);

int run_gui(char const *buffer_name);

void application_on_activate(GtkApplication *app, gpointer user_data);

void update_all_buffer_contents(GtkTextBuffer *buffer,
                                shared_mem_t *shared_memory);

void update_events(user_data_t user_data, shared_mem_t *shared_memory);

#endif // !GUI_H

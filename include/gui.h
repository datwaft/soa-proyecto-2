#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

#include "shared_memory.h"

#define APPLICATION_ID "com.soa.pr02"
#define TEMPLATE_URI "/com/soa/pr02/template.ui"

typedef struct user_data_st {
  GtkBuilder *builder;
} user_data_t;

GtkApplication *application_new(void);

int run_gui(char const *buffer_name);

void application_on_activate(GtkApplication *app);

void update_circbuf_content(shared_mem_t *shared_memory,
                            user_data_t *user_data);

void update_event_history_content(shared_mem_t *shared_memory,
                                  user_data_t *user_data);

#endif // !GUI_H

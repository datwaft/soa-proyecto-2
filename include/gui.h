#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

// Application constants
#define APPLICATION_ID "com.soa.pr02"
#define TEMPLATE_URI "/com/soa/pr02/template.ui"

typedef struct user_data_st {
  GtkBuilder *builder;
} user_data_t;

GtkApplication *application_new(void);

void application_on_activate(GtkApplication *app, gpointer user_data);

#endif // !GUI_H

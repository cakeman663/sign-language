#include <gtk/gtk.h>
#include <glib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define DEFAULT_IMAGE "images/_.jpg"

struct translate_ui_group {
  int            position;
  bool           status;
  GtkImage       *image;
  GtkEntry       *entry;
  GtkLabel       *label;
  GtkButton      *button;
  GtkAdjustment  *adjustment;
  GtkScale       *scale;
};

void set_image(GtkImage *image, char *image_path)
{
  GdkPixbuf *pixbuf;
  GError *error = NULL;

  pixbuf = gdk_pixbuf_new_from_file(image_path, &error);
  
  if (pixbuf == NULL) {
    g_clear_error(&error);
    pixbuf = gdk_pixbuf_new_from_file(DEFAULT_IMAGE, &error);
  }

  gtk_image_set_from_pixbuf(image, pixbuf);
}

void stop_showing(struct translate_ui_group *group) 
{
  group->status = false;

  gtk_button_set_label(group->button, "Start");
  gtk_widget_set_sensitive((GtkWidget *)group->scale, true);
  gtk_label_set_text(group->label, ""); 
  
  set_image(group->image, DEFAULT_IMAGE);
}

guint process_translate(struct translate_ui_group *group) {
  char image_path[13], label_text[2];
  const gchar *text = gtk_entry_get_text(group->entry);  

  sprintf(image_path, "images/%c.jpg", tolower(text[group->position]));
  sprintf(label_text, "%c", tolower(text[group->position]));

  set_image(group->image, image_path);
  gtk_label_set_text(group->label, label_text);

  ++group->position;

  if (strlen(text) < group->position || group->status == false) {
    stop_showing(group);
    return false;
  }
  
  return true;
}

void start_showing(GtkWidget *widget, struct translate_ui_group *group)
{
  if (group->status == false) {
    guint interval = gtk_adjustment_get_value(group->adjustment) * 1000;
    const gchar *text = gtk_entry_get_text(group->entry);  

    if (strlen(text) > 0 && strlen(text) < 255) {
      group->status = true;
      group->position = 0;
     
      gtk_button_set_label(group->button, "Stop");
      gtk_widget_set_sensitive((GtkWidget *)group->scale, false);

      g_timeout_add(interval, (GSourceFunc) process_translate, group);
    }
  } else {
    group->status = false;  
  }
}

int main(int argc, char *argv[])
{
  GtkBuilder *builder;
  GObject *window, *button, *entry, *image, *label, *time, *scale;
  GError *error = NULL;
  struct translate_ui_group *group;

  gtk_init(&argc, &argv);

  builder = gtk_builder_new();
  if (gtk_builder_add_from_file(builder, "main.ui", &error) == 0)
  {
    g_printerr("Error loading file: %s\n", error->message);
    g_clear_error(&error);
    return 1;
  }

  window = gtk_builder_get_object(builder, "window");
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  entry  = gtk_builder_get_object(builder, "input");
  image  = gtk_builder_get_object(builder, "image");
  label  = gtk_builder_get_object(builder, "label"); 
  time   = gtk_builder_get_object(builder, "time_adjustment");
  scale  = gtk_builder_get_object(builder, "scale");
  button = gtk_builder_get_object(builder, "start_button");

  group = (struct translate_ui_group *)g_new(struct translate_ui_group, 1);  
  group->entry      = (GtkEntry *)entry;
  group->image      = (GtkImage *)image;
  group->label      = (GtkLabel *)label;
  group->adjustment = (GtkAdjustment *)time;
  group->scale      = (GtkScale *)scale;
  group->button     = (GtkButton *)button;
  group->status     = false;

  g_signal_connect(button, "clicked", G_CALLBACK(start_showing), group);

  button = gtk_builder_get_object(builder, "close_button");
  g_signal_connect(button, "clicked", G_CALLBACK(gtk_main_quit), NULL);

  set_image(group->image, DEFAULT_IMAGE);

  gtk_main();

  g_free(group);
  return 0;
}

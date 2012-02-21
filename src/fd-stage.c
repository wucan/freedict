#include <gtk/gtk.h>


static GtkWidget * fd_stage_window_get(GtkWidget *do_widget)
{
	static GtkWidget *window = NULL;
	GError *err = NULL;
	gchar *filename = "../ui/stage_window.glade";

	if (!window) {
		GtkBuilder *builder = gtk_builder_new();
		gtk_builder_add_from_file(builder, filename, &err);
		if (err) {
			g_error("ERROR: %s\n", err->message);
			return NULL;
		}
		gtk_builder_connect_signals(builder, NULL);
		window = GTK_WIDGET(gtk_builder_get_object(builder, "window_stage"));
		gtk_window_set_screen(GTK_WINDOW(window), gtk_widget_get_screen(do_widget));
		g_signal_connect(window, "destroy", G_CALLBACK (gtk_widget_destroyed), &window);
	}

	return window;
}

void fd_stage_init()
{
	fd_stage_window_get(NULL);
}

void fd_stage_show()
{
	GtkWidget *stage;

	g_print("stage: show ...\n");

	stage = fd_stage_window_get(NULL);
	if(stage && !gtk_widget_get_visible(stage)) {
		gtk_widget_show_all(stage);
	}
}


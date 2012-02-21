#include <gtk/gtk.h>

#include <X11/Xlib.h>


/*
 * get mouse position, in screen space
 */
static int get_mouse_position(int *x, int *y)
{
	Display *dsp = XOpenDisplay(NULL);
	if (!dsp)
		return -1;

	int screenNumber = DefaultScreen(dsp);
	XEvent event;

	/* get info about current pointer position */
	XQueryPointer(dsp, RootWindow(dsp, DefaultScreen(dsp)),
		&event.xbutton.root, &event.xbutton.window,
		&event.xbutton.x_root, &event.xbutton.y_root,
		&event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

	printf("Mouse Coordinates: %d %d\n", event.xbutton.x, event.xbutton.y);

	*x = event.xbutton.x;
	*y = event.xbutton.y;

	XCloseDisplay(dsp);

	return 0;
}


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
	int x, y;

	g_print("stage: show ...\n");

	stage = fd_stage_window_get(NULL);

	/*
	 * there are alterntive but too simple method to set window position:
	 * gtk_window_set_position(GTK_WINDOW(stage), GTK_WIN_POS_MOUSE);
	 * use _move() is the answer.
	 */
	get_mouse_position(&x, &y);
	gtk_window_move(GTK_WINDOW(stage), x, y);

	if(!gtk_widget_get_visible(stage)) {
		gtk_widget_show_all(stage);
	}
}


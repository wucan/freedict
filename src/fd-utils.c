#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <glib.h>
#include <gtk/gtk.h>


static Window Window_With_Name(Display *dpy, Window top, const char *name);
static Window find_window_with_name(Display *display, const char *name);

gchar * fd_utils_get_active_window_title()
{
	Status status;
	Display *display;
	Window focus;
	char *window_name = NULL;
	int revert;
	int rc;

	display = XOpenDisplay(NULL);
	rc = XGetInputFocus(display, &focus, &revert);
	if (!rc) {
		g_print("XGetInputFocus() failed!\n");
		goto done;
	}

	/*
	 * get the window title/name
	 * FIXME: not working!
	 */
#if 0
	status = XFetchName(display, focus, &window_name);
#else
	XTextProperty text_prop;
	status = XGetWMName(display, focus, &text_prop);
	if (status) {
		char **str_list;
		int str_cnt;
		status = XTextPropertyToStringList(&text_prop, &str_list, &str_cnt);
		if (status == 0 && str_cnt > 0) {
			window_name = g_strdup(str_list[0]);
			XFreeStringList(str_list);
		}
	} else {
		g_print("XGetWMName() failed!\n", status);
	}
#endif

done:
	XCloseDisplay(display);

	if (!window_name)
		window_name = g_strdup("Unknown Context!");

	return window_name;
}

/*
 * Routine steal form xprop.
 *
 * Window_With_Name: routine to locate a window with a given name on a display.
 *									 If no window with the given name is found, 0 is returned.
 *									 If more than one window has the given name, the first
 *									 one found will be returned.	Only top and its subwindows
 *									 are looked at.	Normally, top should be the RootWindow.
 */
static Window Window_With_Name(Display *dpy, Window top, const char *name)
{
	Window *children, dummy;
	unsigned int nchildren;
	int i;
	Window w=0;
	char *window_name;

	if (XFetchName(dpy, top, &window_name) && !strcmp(window_name, name))
		return(top);

	if (!XQueryTree(dpy, top, &dummy, &dummy, &children, &nchildren))
		return(0);

	for (i=0; i<nchildren; i++) {
		w = Window_With_Name(dpy, children[i], name);
		if (w)
			break;
	}

	if (children) XFree((char *)children);

	return(w);
}

static Window find_window_with_name(Display *display, const char *name)
{
	return Window_With_Name(display, RootWindow(display, 0), name);
}

/*
 * get mouse position, in screen space
 */
int x11_mouse_position(int *x, int *y)
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

	*x = event.xbutton.x;
	*y = event.xbutton.y;

	XCloseDisplay(dsp);

	return 0;
}

int gdk_get_mouse_position(int *x, int *y)
{
	GdkDisplay *display = gdk_display_get_default();

	/* get cursor position */
	gdk_display_get_pointer(display, NULL, x, y, NULL);

	return 0;
}

gboolean is_mouse_nearby_stage_window(GtkWidget *stage)
{
	gint x, y;
	gint win_x, win_y, win_w, win_h;
	GdkDisplay *display;
	GdkWindow *window;

	if(!gtk_widget_get_visible(stage))
		return FALSE;

	display = gdk_display_get_default();
	gdk_display_get_pointer(display, NULL, &x, &y, NULL);

	gtk_window_get_position(GTK_WINDOW(stage), &win_x, &win_y);
	gtk_window_get_size(GTK_WINDOW(stage), &win_w, &win_h);

	GdkRectangle win_rect = {win_x, win_y, win_w, win_h};
	GdkRectangle mouse_rect = {x - 5, y - 5, 10, 10};
	if (gdk_rectangle_intersect(&win_rect, &mouse_rect, NULL))
		return TRUE;
	else
		return FALSE;
}


#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <glib.h>
#include <gtk/gtk.h>


static Display *display;

static Window Window_With_Name(Display *dpy, Window top, const char *name);
static Window find_window_with_name(Display *display, const char *name);

gboolean fd_utils_init()
{
	display = XOpenDisplay(NULL);
	if (!display) {
		g_print("XOpenDisplay() failed!\n");
		return FALSE;
	}

	return TRUE;
}

void fd_utils_deinit()
{
	if (display) {
		XCloseDisplay(display);
		display = NULL;
	}
}

static Window fix_window(Display *display, Window win, int revert)
{
#if 0
	Status status;
	Atom *protocols;
	int cnt;

	status = XGetWMProtocols(display, win, &protocols, &cnt);
	if (status == 0) {
		/* FIXME: Why the focus window id is larger by 1!!! */
		g_print("fix window from %#x -> %#x\n", win, win - 1);
		win -= 1;
	} else {
		XFree(protocols);
	}
#else
	/* revert meaning ??? ! */
	win -= revert - 1;
#endif

	return win;
}

static gchar * fd_utils_x11_get_active_window_title()
{
	Status status;
	Window focus;
	char *window_name = NULL;
	int revert;
	int rc;

	rc = XGetInputFocus(display, &focus, &revert);
	if (!rc) {
		g_print("XGetInputFocus() failed!\n");
		goto done;
	}

	focus = fix_window(display, focus, revert);

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
		if (status && str_cnt > 0) {
			window_name = g_strdup(str_list[0]);
			XFreeStringList(str_list);
		}
	} else {
		g_print("XGetWMName() failed!\n", status);
	}
#endif

done:

	return window_name;
}

gchar * fd_utils_x11_get_active_window_title_v2()
{
	Status status;
	Window focus;
	char *window_name = NULL;
	int revert;
	int rc;

	rc = XGetInputFocus(display, &focus, &revert);
	if (!rc) {
		g_print("XGetInputFocus() failed!\n");
		goto done;
	}

	focus = fix_window(display, focus, revert);

	const char *prop;
	Atom atom, type;
	int format;
	long length;
	unsigned long nitems, nbytes, nbytes_after;
	int size;

	atom = XInternAtom(display, "_NET_WM_NAME", True);
	if (atom == None) {
		g_print("WM_NAME atom not found!\n");
		goto done;
	}
	status = XGetWindowProperty(display, focus, atom, 0, 100000,
			False, AnyPropertyType, &type,
			&format, &nitems, &nbytes_after, &prop);
	if (status == BadWindow) {
		g_print("BadWindow! window id %#x not exist!", focus);
	} else if (status != Success) {
		g_print("XGetWindowProperty failed! (%d)\n", status);
	}

	//g_print("type %d, format %d, nitems %d, nbytes_after %d, prop %p\n",
	//	type, format, nitems, nbytes_after, prop);
	switch (format) {
		case 32: nbytes = sizeof(long); break;
		case 16: nbytes = sizeof(short); break;
		case 8: nbytes = sizeof(char); break;
		case 0: nbytes = 0; break;
		default: goto done; break;
	}
	length = nitems * nbytes;
	size = format;
	window_name = prop;

done:

	return window_name;
}

static gchar * fd_utils_gdk_get_active_window_title()
{
	GdkScreen *screen;
	GdkWindow *act_win, *top_win;
	gchar *title = NULL;

	screen = gdk_screen_get_default();
	act_win = gdk_screen_get_active_window(screen);
	if (act_win) {
		top_win = gdk_window_get_effective_toplevel(act_win);
		if (top_win) {
			/* TODO: */
		}
	}
	if (act_win)
		g_object_unref(act_win);

	return title;
}

gchar * fd_utils_get_active_window_title()
{
	gchar *title;

	title = fd_utils_x11_get_active_window_title();
	if (!title)
		title = fd_utils_x11_get_active_window_title_v2();
	//title = fd_utils_gdk_get_active_window_title();

	if (!title)
		title = g_strdup("Unknown Title!");

	return title;
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
	int screenNumber = DefaultScreen(display);
	XEvent event;

	/* get info about current pointer position */
	XQueryPointer(display, RootWindow(display, DefaultScreen(display)),
		&event.xbutton.root, &event.xbutton.window,
		&event.xbutton.x_root, &event.xbutton.y_root,
		&event.xbutton.x, &event.xbutton.y, &event.xbutton.state);

	*x = event.xbutton.x;
	*y = event.xbutton.y;

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

void hex_dump(const gchar *prefix, guchar *buf, int size)
{
	int i;

	if (size < 0)
		return;
	g_print("%s:", prefix);
	for (i = 0; i < size; i++)
		g_print(" %02x", buf[i]);
	g_print("\n");
}


#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <glib.h>


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


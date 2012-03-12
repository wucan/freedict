#include <gdk/gdk.h>

#include "gdk_support.h"


GdkPixbuf *create_pixbuf(const char *filename)
{
	gchar *path;
	GdkPixbuf *pixbuf;
	GError *err = NULL;

	if (!filename || !filename[0])
		return NULL;
	path = "./../pixmaps/freedict.png";
	pixbuf = gdk_pixbuf_new_from_file(path, &err);
	if (!pixbuf) {
		g_error_free(err);
	}

	return pixbuf;
}


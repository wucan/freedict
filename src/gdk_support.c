#include <gdk/gdk.h>
#include <gnome.h>

#include "gdk_support.h"


GdkPixbuf *create_pixbuf(const char *filename)
{
	gchar *path;
	GdkPixbuf *pixbuf;
	GError *err = NULL;

	if (!filename || !filename[0])
		return NULL;
#if 0
	path = gnome_program_locate_file(NULL, GNOME_FILE_DOMAIN_APP_PIXMAP,
				filename, TRUE, NULL);
	if (!path) {
		g_warning(_("Couldn't locate pixmap file: %s"), filename);
		return NULL;
	}
#else
	path = "./../pixmaps/ipmsg.xpm";
#endif
	pixbuf = gdk_pixbuf_new_from_file(path, &err);
	if (!pixbuf) {
		g_error_free(err);
	}
	//g_free(path);

	return pixbuf;
}


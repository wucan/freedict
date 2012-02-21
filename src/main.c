#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnome.h>

#include "fd-systray.h"
#include "fd-stage.h"


/*
 * GNOME_PROGRAM_STANDARD_PROPERTIES,
 */
#define PREFIX		"/usr/local"
#define SYSCONFDIR	"/usr/local/etc"
#define LIBDIR		"/usr/local/lib"
#define DATADIR		"/usr/local/share"

#define PACKAGE_DATA_DIR		"/projects/freedict"

int main(int argc, char **argv)
{
	gnome_program_init(PACKAGE, "1.0", LIBGNOMEUI_MODULE, argc, argv,
		GNOME_PROGRAM_STANDARD_PROPERTIES,
		GNOME_PARAM_APP_DATADIR,
		PACKAGE_DATA_DIR, NULL);

	fd_systray_create();

	GtkWidget *stage = fd_stage_window_get(NULL);
	if(!gtk_widget_get_visible(stage)) {
		gtk_widget_show_all(stage);
	}

	gtk_main();

	return 0;
}


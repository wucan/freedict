#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnome.h>

#include "fd-systray.h"
#include "fd-stage.h"
#include "fd-clipboard.h"
#include "fd-dict.h"


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
	XInitThreads();

	/* GtkBuilder signals need this */
	GModule *mod = g_module_open(NULL, G_MODULE_BIND_LOCAL);
	if (!mod) {
		g_print("module open failed!\n");
	}
	gpointer method;
	g_module_symbol(mod, "button_save_clicked", &method);
	if (method) {
		g_print("found method %p\n", method);
	}

	gnome_program_init(PACKAGE, "1.0", LIBGNOMEUI_MODULE, argc, argv,
		GNOME_PROGRAM_STANDARD_PROPERTIES,
		GNOME_PARAM_APP_DATADIR,
		PACKAGE_DATA_DIR, NULL);

	fd_dict_init();

	fd_systray_create();

	fd_stage_init();

	fd_clipboard_init();

	gtk_main();

	return 0;
}


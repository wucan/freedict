#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <X11/Xlib.h>

#include <stdlib.h>

#include "fd-systray.h"
#include "fd-stage.h"
#include "fd-clipboard.h"
#include "fd-dict.h"
#include "fd-user-dict.h"
#include "fd-utils.h"


int main(int argc, char **argv)
{
	printf("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);

	Status status = XInitThreads();
	if (!status) {
		g_print("XInitThreads() failed!\n");
		exit(EXIT_FAILURE);
	}

	gtk_init(&argc, &argv);

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

	fd_utils_init();
	fd_dict_init();
	fd_user_dict_open();

	fd_systray_create();

	fd_stage_init();

	fd_clipboard_init();

	gtk_main();

	fd_user_dict_close();
	fd_utils_deinit();

	return 0;
}


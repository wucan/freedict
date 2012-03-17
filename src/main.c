#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "fd-systray.h"
#include "fd-stage.h"
#include "fd-clipboard.h"
#include "fd-dict.h"
#include "fd-user-dict.h"
#include "fd-utils.h"
#include "fd-iciba.h"


int main(int argc, char **argv)
{
	printf("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);

	gtk_init(&argc, &argv);

	g_set_application_name(PACKAGE_NAME " " PACKAGE_VERSION);

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
	fd_iciba_init();

	fd_systray_create();

	fd_stage_init();

	fd_clipboard_init();

	gtk_main();

	fd_clipboard_deinit();
	fd_iciba_deinit();
	fd_user_dict_close();
	fd_utils_deinit();

	return 0;
}


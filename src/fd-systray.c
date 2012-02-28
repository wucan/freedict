#include <glib.h>
#include "gtk_support.h"
#include "gdk_support.h"
#include "fd-window.h"


static void preferences_menu_item_activate_cb(gpointer menu_item)
{
	show_preferences_window(NULL);
}
static void quit_menu_item_activate_cb(gpointer menu_item)
{
	gtk_main_quit();
}

static void pickable_menu_item_toggled_cb(GtkCheckMenuItem *item)
{
	gboolean pickable;

	pickable = !gtk_check_menu_item_get_active(item);

	/* TODO */
}

static GtkWidget *create_main_menu()
{
	GtkWidget *menu;
	GtkWidget *preferences_item;
	GtkWidget *pickable_item;
	GtkWidget *quit_item;

	menu = gtk_menu_new();
	preferences_item = create_stock_menu_item("preferences_item",
		GTK_STOCK_PREFERENCES, NULL, preferences_menu_item_activate_cb);
	gtk_menu_append(GTK_MENU(menu), preferences_item);
	pickable_item = create_check_menu_item("pickable_item", "Pickable",
			pickable_menu_item_toggled_cb);
	gtk_menu_append(GTK_MENU(menu), pickable_item);
	quit_item = create_menu_item("quit_item", "Quit",
			NULL, quit_menu_item_activate_cb);
	gtk_menu_append(GTK_MENU(menu), quit_item);
	GLADE_HOOKUP_OBJECT(menu, preferences_item, "preferences_item");
	GLADE_HOOKUP_OBJECT(menu, quit_item, "quit_item");

	return menu;
}
static void status_icon_activate_cb(GtkStatusIcon *status_icon,
		gpointer user_data)
{
	show_main_window(NULL);
}
static void status_icon_popup_menu_cb(GtkStatusIcon *status_icon,
 		guint button, guint activate_time, gpointer user_data)
{
	gtk_menu_popup(GTK_MENU(create_main_menu()), NULL, NULL,
		gtk_status_icon_position_menu, status_icon, button,activate_time);
}
GtkWidget *fd_systray_create()
{
	GtkStatusIcon *status_icon = NULL;
	GdkPixbuf *pixbuf;

	pixbuf = create_pixbuf ("pixmaps/ipmsg.xpm");
	if (pixbuf) {
			status_icon = gtk_status_icon_new_from_pixbuf(pixbuf);
			gdk_pixbuf_unref(pixbuf);
	}

	g_signal_connect((gpointer)status_icon,
		"activate", G_CALLBACK (status_icon_activate_cb), NULL);
	g_signal_connect((gpointer)status_icon,
		"popup-menu", G_CALLBACK(status_icon_popup_menu_cb), NULL);

	gtk_status_icon_set_tooltip(status_icon, "Freedict");

	GLADE_HOOKUP_OBJECT_NO_REF(status_icon, status_icon, "status_icon");

	return (GtkWidget *)status_icon;
}


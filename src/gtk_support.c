#include <gtk/gtk.h>

#include "gtk_support.h"


GtkWidget *create_menu_item(const char *name, const char *label,
		gpointer data, void (*menu_item_callbacker)(gpointer user_data))
{
	GtkWidget *menu_item;
	int index = (int)data;

	menu_item = gtk_menu_item_new_with_label(label);
	if (menu_item_callbacker) {
		if (data) {
			gtk_signal_connect_object((gpointer)menu_item, "activate",
				GTK_SIGNAL_FUNC(menu_item_callbacker), data);
		} else {
			gtk_signal_connect_object((gpointer)menu_item, "activate",
				GTK_SIGNAL_FUNC(menu_item_callbacker), menu_item);
		}
	}
	gtk_widget_show(menu_item);

	return menu_item;
}
GtkWidget *create_stock_menu_item(const char *name, const gchar *stock_id,
		gpointer data, void (*menu_item_callbacker)(gpointer user_data))
{
	GtkWidget *menu_item;
	int index = (int)data;

	menu_item = gtk_image_menu_item_new_from_stock(stock_id, NULL);
	if (menu_item_callbacker) {
		if (data) {
			gtk_signal_connect_object((gpointer)menu_item, "activate",
				GTK_SIGNAL_FUNC(menu_item_callbacker), data);
		} else {
			gtk_signal_connect_object((gpointer)menu_item, "activate",
				GTK_SIGNAL_FUNC(menu_item_callbacker), menu_item);
		}
	}
	gtk_widget_show(menu_item);

	return menu_item;
}

GtkWidget *create_check_menu_item(const char *name, const char *label,
		void (*toggled_callbacker)(GtkCheckMenuItem *item))
{
	GtkWidget *menu_item;

	menu_item = gtk_check_menu_item_new_with_label(label);
	gtk_check_menu_item_set_active((GtkCheckMenuItem *)menu_item, TRUE);
	gtk_check_menu_item_set_show_toggle((GtkCheckMenuItem *)menu_item, TRUE);
	gtk_signal_connect_object((gpointer)menu_item, "toggled",
		GTK_SIGNAL_FUNC(toggled_callbacker), menu_item);
	gtk_widget_show(menu_item);

	return menu_item;
}


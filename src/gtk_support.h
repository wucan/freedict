#ifndef _GTK_SUPPORT_H_
#define _GTK_SUPPORT_H_

#include <gtk/gtk.h>


/*
 * store pointers and use by lookup_widget()
 */
#define GLADE_HOOKUP_OBJECT(component,widget,name) \
	g_object_set_data_full(G_OBJECT (component), name, \
		gtk_widget_ref(widget), (GDestroyNotify)gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
	g_object_set_data(G_OBJECT(component), name, widget)


GtkWidget *create_menu_item(const char *name, const char *label,
		gpointer data, void (*menu_item_activate_cb)(gpointer user_data));
GtkWidget *create_stock_menu_item(const char *name, const gchar *stock_id,
		gpointer data, void (*menu_item_activate_cb)(gpointer user_data));
GtkWidget *create_check_menu_item(const char *name, const char *label,
		void (*toggled_callbacker)(GtkCheckMenuItem *item));


#endif /* _GTK_SUPPORT_H_ */


#include <gtk/gtk.h>
#include <gnome.h>


GtkWidget *fd_window_new()
{
	GtkWidget *fd_window;

	return NULL;
}
static void preferences_window_response_cb(GtkDialog *dialog,
	gint response, void *data)
{
	gtk_widget_destroy(GTK_WIDGET(dialog));
}
/*
 * load preferences window from?
 */
void show_preferences_window(GtkWidget *fd_window)
{
	GtkWidget *dialog;

	dialog = gtk_dialog_new_with_buttons(_("Preferences Editor"),
		GTK_WINDOW(fd_window), 	GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_CLOSE);
	gtk_container_set_border_width(GTK_CONTAINER(GTK_DIALOG(dialog)), 5);
	gtk_box_set_spacing(GTK_BOX(GTK_DIALOG(dialog)->vbox), 2);
	gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
	gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 400);
	g_signal_connect(dialog, "response",
		G_CALLBACK(preferences_window_response_cb), fd_window);
	gtk_widget_show_all(dialog);
}
/*
 * load main window from?
 */
void show_main_window(GtkWidget *fd_window)
{
	GtkWidget *dialog;
	GtkWidget *word_entry;
	GtkWidget *result;

	dialog = gtk_dialog_new();
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_CLOSE);
	gtk_container_set_border_width(GTK_CONTAINER(GTK_DIALOG(dialog)), 5);
	gtk_box_set_spacing(GTK_BOX(GTK_DIALOG(dialog)->vbox), 2);
	gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
	gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 400);

	word_entry = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), word_entry);
	result = gtk_text_view_new();
	gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), result);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(result), FALSE);

	gtk_widget_show_all(dialog);
}


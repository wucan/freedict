#include <gtk/gtk.h>


static GtkClipboard *clipboard;

gchar * fd_clipboard_get()
{
	return NULL;
}

static gchar *recv_text;
static gboolean rearm = TRUE;

static void receiver_func(GtkClipboard *clipboard,
		const gchar *text, gpointer data)
{
	if (text && recv_text && strcmp(recv_text, text) == 0) {
		rearm = TRUE;
		return;
	}

	if (recv_text) {
		g_free(recv_text);
		recv_text = NULL;
	}

	if (text)
		recv_text = g_strdup(text);

	if (recv_text)
		g_print("clipboard text: %s\n", recv_text);

	rearm = TRUE;
}

static void thread_func(gpointer data)
{
	while (TRUE) {
		if (rearm) {
			gtk_clipboard_request_text(clipboard, receiver_func, NULL);
			rearm = FALSE;
		}
		usleep(100000);
	}
}

void fd_clipboard_init()
{
	/* tell the clipboard manager to make the data persistent */
	clipboard = gtk_clipboard_get(GDK_SELECTION_PRIMARY);
	/* GDK_SELECTION_PRIMARY, CLIPBOARD */
	gtk_clipboard_set_can_store(clipboard, NULL, 0);
	g_thread_create(thread_func, NULL, FALSE, NULL);
}



#include <gtk/gtk.h>

#include "fd-dict.h"
#include "fd-user-dict.h"
#include "fd-lookup.h"
#include "fd-utils.h"


static GtkTextView *textview_content;
static GtkEntry *entry_word;
static GtkWidget *button_edit;

static struct fd_lookup_context lookup_ctx = {0};

static void update_content(const gchar *text, const gchar *context);

void button_save_clicked(GtkWidget *widget,
			GdkEventButton *event, gpointer data)
{
	gchar *word, *content;
	GtkTextBuffer *text_buf;
	GtkTextIter start, end;

	word = gtk_entry_get_text(entry_word);
	text_buf = gtk_text_view_get_buffer(textview_content);
	gtk_text_buffer_get_start_iter(text_buf, &start);
	gtk_text_buffer_get_end_iter(text_buf, &end);
	content = gtk_text_buffer_get_text(text_buf, &start, &end, FALSE);

	fd_user_dict_add(word, content, lookup_ctx.context);

	g_free(content);

	fd_stage_unpin();
}

static void button_edit_clicked(GtkWidget *widget,
			GdkEventButton *event, gpointer data)
{
	gboolean editable;

	editable = !gtk_text_view_get_editable(textview_content);
	gtk_text_view_set_editable(textview_content, editable);

	if (editable) {
		gtk_button_set_label(GTK_BUTTON(widget), "Editing");
		fd_stage_pin();
	} else {
		gtk_button_set_label(GTK_BUTTON(widget), "Edit");
		fd_stage_unpin();
	}
}

static void entry_word_changed(GtkEditable *editable, gpointer user_data)
{
	const gchar *word;

	word = gtk_entry_get_text(entry_word);
	if (word && word[0])
		update_content(word, "User Input");
}

static GtkWidget * fd_stage_window_get(GtkWidget *do_widget)
{
	static GtkWidget *window = NULL;
	GError *err = NULL;
	gchar *filename = "../ui/stage_window.glade";

	if (!window) {
		GtkBuilder *builder = gtk_builder_new();
		gtk_builder_add_from_file(builder, filename, &err);
		if (err) {
			g_error("ERROR: %s\n", err->message);
			return NULL;
		}
		gtk_builder_connect_signals(builder, NULL);
		window = GTK_WIDGET(gtk_builder_get_object(builder, "window_stage"));
		if (do_widget)
			gtk_window_set_screen(GTK_WINDOW(window), gtk_widget_get_screen(do_widget));
		g_signal_connect(window, "delete-event", G_CALLBACK (gtk_widget_hide_on_delete), window);
		/*
		 * always on top
		 */
		gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
		if (gtk_widget_is_composited(window)) {
			gdouble opacity = 0.2;
			gtk_window_set_opacity(GTK_WINDOW(window), opacity);
		}

		textview_content = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "textview_content"));

		/* hook up signals */
		/* FIXME: not working! */
		/* gtk_builder_connect_signals(builder, NULL); */
		GtkWidget *button_save = gtk_builder_get_object(builder, "button_save");
		button_edit = gtk_builder_get_object(builder, "button_edit");
		entry_word = GTK_ENTRY(gtk_builder_get_object(builder, "entry_word"));
		g_signal_connect(G_OBJECT(button_save), "clicked",
			G_CALLBACK(button_save_clicked), NULL);
		g_signal_connect(G_OBJECT(button_edit), "clicked",
			G_CALLBACK(button_edit_clicked), NULL);
		g_signal_connect(G_OBJECT(entry_word), "changed",
			G_CALLBACK(entry_word_changed), NULL);
	}

	return window;
}

void fd_stage_init()
{
	fd_stage_window_get(NULL);
}

static guint timer_id;

static gboolean timeout_func(gpointer data)
{
	GtkWidget *stage = data;

	if (!is_mouse_nearby_stage_window(stage)) {
		gtk_widget_hide(stage);
		timer_id = 0;
		return FALSE;
	}

	return TRUE;
}

static void update_content(const gchar *text, const gchar *context)
{
	GtkTextBuffer *text_buf;
	char buf[1024];
	gchar *answer;

	fd_lookup_context_destroy(&lookup_ctx);
	fd_lookup_context_init(&lookup_ctx, text, context);

	/* update entry_word */
	gtk_entry_set_text(entry_word, text);
	/* update textview_content */
	text_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_content));
	fd_lookup_exec(&lookup_ctx);
	answer = fd_lookup_context_build_answer(&lookup_ctx);
	sprintf(buf, "%s", answer);
	gtk_text_buffer_set_text(text_buf, buf, -1);
}

void fd_stage_show(const gchar *text, const gchar *context)
{
	GtkWidget *stage;
	int x, y;

	stage = fd_stage_window_get(NULL);

	/* ignore new coming if current in editable mode */
	if (gtk_widget_get_visible(stage) &&
		gtk_text_view_get_editable(textview_content))
		return;

	if (gtk_text_view_get_editable(textview_content)) {
		gtk_button_set_label(GTK_BUTTON(button_edit), "Edit");
		gtk_text_view_set_editable(textview_content, FALSE);
	}

	/*
	 * update contents
	 */
	update_content(text, context);

	if (!is_mouse_nearby_stage_window(stage)) {
		/*
		 * there are alterntive but too simple method to set window position:
		 * gtk_window_set_position(GTK_WINDOW(stage), GTK_WIN_POS_MOUSE);
		 * use _move() is the answer.
		 */
		gdk_get_mouse_position(&x, &y);
		gtk_window_move(GTK_WINDOW(stage), x, y);
	}

	if(!gtk_widget_get_visible(stage)) {
		gtk_widget_show_all(stage);
	}

	if (timer_id > 0) {
		g_source_remove(timer_id);
		timer_id = 0;
	}

	timer_id = g_timeout_add(100, timeout_func, stage);
}

void fd_stage_pin()
{
	if (timer_id > 0) {
		g_source_remove(timer_id);
		timer_id = 0;
	}
}

void fd_stage_unpin()
{
	GtkWidget *stage = fd_stage_window_get(NULL);

	if (timer_id > 0) {
		g_source_remove(timer_id);
		timer_id = 0;
	}

	if(gtk_widget_get_visible(stage)) {
		gtk_widget_hide(stage);
	}
}


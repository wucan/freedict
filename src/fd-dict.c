#include <glib.h>


struct fd_dict {
	gchar *uri;
	gchar *data;
	gsize size;

	gchar * (*get_answer)(const gchar *words);
};

struct fd_dict_box {
	GList *dicts;

};

static struct fd_dict_box dict_box;
static struct fd_dict *cur_dict;


static gchar * get_answer(const char *words)
{
	/* TODO */

	return "Not Implement";
}

gchar * fd_dict_get_answer(const gchar *words)
{
	return cur_dict->get_answer(words);
}

static struct fd_dict * fd_dict_load_dict(gchar *uri)
{
	struct fd_dict *dict;
	GError *err;
	gboolean rb;

	/*
	 * load dictionary data from uri
	 */
	dict = (struct fd_dict *)g_malloc0(sizeof(*dict));
	dict->uri = g_strdup(uri);
	rb = g_file_get_contents(uri, &dict->data, &dict->size, &err);
	if (!rb) {
		if (err) {
			g_print("load dict failed! (%s)\n", err->message);
			g_error_free(err);
		}
		g_free(dict);
		return NULL;
	}

	/*
	 * prepare the dictionry access method, and preparse for quick search?
	 */
	dict->get_answer = get_answer;

	return dict;
}

void fd_dict_init()
{
	struct fd_dict *dict;

	dict_box.dicts = g_list_alloc();

	dict = fd_dict_load_dict("../dicts/ldc_ec_dict.2.0.txt");
	dict_box.dicts = g_list_append(dict_box.dicts, dict);

	cur_dict = dict;
}


#include <glib.h>


struct fd_dict {
	gchar *uri;
	gchar *char_encode;
	gchar *data;
	gsize size;

	gchar * (*get_answer)(const gchar *words);
};

struct fd_dict_box {
	GList *dicts;

};

static struct fd_dict_box dict_box;
static struct fd_dict *cur_dict;

static void hex_dump(const gchar *prefix, guchar *buf, int size)
{
	int i;

	if (size < 0)
		return;
	g_print("%s:", prefix);
	for (i = 0; i < size; i++)
		g_print(" %02x", buf[i]);
	g_print("\n");
}

static gchar * do_conv(gchar *in_encode, gchar *in, gsize in_size)
{
	GIConv iconv = g_iconv_open("UTF-8", in_encode);
	if (iconv == (GIConv)-1)
		return g_strdup("Unknow raw input text!");

	gsize out_size = in_size * 2;
	gsize out_left = out_size;
	gchar *out = g_malloc(out_size);
	gchar *out_head = out;

	g_iconv(iconv, &in, &in_size, &out, &out_left);

	g_iconv_close(iconv);

	return out_head;
}

static gchar * get_answer(const char *words)
{
	GRegex *reg;
	GMatchInfo *match_info;
	gint start = -1, end = -1;
	gchar pat[1024];

	sprintf(pat, "\n%s\t", g_strstrip(words));
	reg = g_regex_new(pat, 0, 0, NULL);
	g_regex_match(reg, cur_dict->data, 0, &match_info);
	while (g_match_info_matches(match_info)) {
		gchar *word = g_match_info_fetch(match_info, 0);
		g_match_info_fetch_pos(match_info, 0, &start, &end);
		g_print("Found: %s, start %d, end %d\n", word, start, end);
		g_free(word);
		g_match_info_next(match_info, NULL);

		/* use the first matched one! */
		break;
	}
	g_match_info_free(match_info);
	g_regex_unref(reg);

	if (start > 0 && end > 0) {
		int tail;
		for (tail = end; cur_dict->data[tail] != '\n'; tail++)
			;
		return do_conv(cur_dict->char_encode, &cur_dict->data[end], tail - end);
	}

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
	dict->char_encode = "GB18030";
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

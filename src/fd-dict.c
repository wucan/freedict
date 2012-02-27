#include <glib.h>

#include "fd-common.h"
#include "fd-str.h"


struct fd_dict_box {
	GList *dicts;

};

static struct fd_dict_box dict_box;
static struct fd_dict *cur_dict;

struct dict_indexer {
	gchar *head, *tail;
};

static struct dict_indexer indexer[0x80];

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

	/* mark out stream end! */
	*out = 0;

	return out_head;
}

static void dict_build_index(struct fd_dict *dict)
{
	gchar *lines = dict->data;
	gchar *line;
	gchar c, last_c = 0;

	while (line = fd_str_next_line_pointer(&lines)) {
		c = line[0];

		if (c != last_c) {
			indexer[c].head = line;
			if (indexer[last_c].head)
				indexer[last_c].tail = line - 1;
			last_c = c;
		}
	}

	if (indexer[last_c].head)
		indexer[last_c].tail = lines;
}

static gchar * get_answer(const char *words)
{
	GRegex *reg;
	GMatchInfo *match_info;
	gint start = -1, end = -1;
	gchar pat[1024];
	gchar *data_head, *data_tail;

	if ((guchar)words[0] >= 0x80 || !indexer[words[0]].head) {
		return NULL;
	}

	sprintf(pat, "%s\t", words);
	reg = g_regex_new(pat, 0, 0, NULL);
	if (!reg) {
		g_print("failed to create regex! words: %s\n", words);
		return NULL;
	}

	data_head = indexer[words[0]].head;
	data_tail = indexer[words[0]].tail;
	*data_tail = 0;

	g_regex_match(reg, data_head, 0, &match_info);
	while (g_match_info_matches(match_info)) {
		gchar *word = g_match_info_fetch(match_info, 0);
		g_match_info_fetch_pos(match_info, 0, &start, &end);
		g_free(word);
		g_match_info_next(match_info, NULL);

		/* use the first matched one! */
		break;
	}
	g_match_info_free(match_info);
	g_regex_unref(reg);

	*data_tail = '\n';

	if (start > 0 && end > 0) {
		int tail;
		for (tail = end; data_head[tail] != '\n'; tail++)
			;
		return do_conv(cur_dict->char_encode, &data_head[end], tail - end);
	}

	return NULL;
}

gchar * fd_dict_get_answer(const gchar *words)
{
	gchar *answer;

	answer = cur_dict->get_answer(words);

	return answer;
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

	dict_build_index(dict);

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


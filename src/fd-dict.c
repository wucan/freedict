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

	/* mark out stream end! */
	*out = 0;

	return out_head;
}

static gchar * get_answer(const char *words)
{
	GRegex *reg;
	GMatchInfo *match_info;
	gint start = -1, end = -1;
	gchar pat[1024];

	sprintf(pat, "\n%s\t", words);
	reg = g_regex_new(pat, 0, 0, NULL);
	if (!reg) {
		g_print("failed to create regex! words: %s\n", words);
		return NULL;
	}
	g_regex_match(reg, cur_dict->data, 0, &match_info);
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

	if (start > 0 && end > 0) {
		int tail;
		for (tail = end; cur_dict->data[tail] != '\n'; tail++)
			;
		return do_conv(cur_dict->char_encode, &cur_dict->data[end], tail - end);
	}

	return NULL;
}

static gchar * do_get_answer(const gchar *words)
{
	gchar *answer;

	answer = cur_dict->get_answer(words);
	if (!answer) {
		gchar *w;
		w = g_ascii_strdown(words, -1);
		answer = cur_dict->get_answer(w);
		g_free(w);
		if (!answer) {
			w = g_ascii_strup(words, -1);
			answer = cur_dict->get_answer(w);
			g_free(w);
		}
	}

	return answer;
}

static gchar * _fd_dict_get_answer(const gchar *words)
{
	gchar *answer;

	answer = do_get_answer(words);
	if (!answer) {
		int words_len = strlen(words);
		gchar *tmp_words = g_strdup(words);
		int suffix_idx = -1;

		do {
			switch (suffix_idx) {
			case -1:
				if (g_str_has_suffix(tmp_words, "ies")) {
					tmp_words[words_len - 2] = 0;
					tmp_words[words_len - 3] = 'y';
					answer = do_get_answer(tmp_words);
					tmp_words[words_len - 2] = 'e';
					tmp_words[words_len - 3] = 'i';
				}
				break;
			case 0:
				if (g_str_has_suffix(tmp_words, "s")) {
					tmp_words[words_len - 1] = 0;
					answer = do_get_answer(tmp_words);
					/* continue search the answer from here! */
					if (!answer)
						 answer = _fd_dict_get_answer(tmp_words);
					tmp_words[words_len - 1] = 's';
				}
				break;
			case 1:
				if (g_str_has_suffix(tmp_words, "es")) {
					tmp_words[words_len - 1] = 0;
					tmp_words[words_len - 2] = 0;
					answer = do_get_answer(tmp_words);
					tmp_words[words_len - 1] = 's';
					tmp_words[words_len - 2] = 'e';
				}
				break;
			case 2:
				if (g_str_has_suffix(tmp_words, "er")) {
					tmp_words[words_len - 1] = 0;
					tmp_words[words_len - 2] = 0;
					answer = do_get_answer(tmp_words);
					tmp_words[words_len - 1] = 'r';
					tmp_words[words_len - 2] = 'e';
				}
				break;
			case 3:
				if (g_str_has_suffix(tmp_words, "ing")) {
					tmp_words[words_len - 1] = 0;
					tmp_words[words_len - 2] = 0;
					tmp_words[words_len - 3] = 0;
					answer = do_get_answer(tmp_words);
					tmp_words[words_len - 1] = 'g';
					tmp_words[words_len - 2] = 'n';
					tmp_words[words_len - 3] = 'i';
				}
				break;
			case 4:
				if (g_str_has_suffix(tmp_words, "ed")) {
					tmp_words[words_len - 1] = 0;
					answer = do_get_answer(tmp_words);
					tmp_words[words_len - 1] = 'd';
				}
				break;
			default:
				answer = g_strdup("Not Found!");
				break;
			}
			suffix_idx++;
		} while (!answer);
done:
		g_free(tmp_words);
	}

	return answer;
}

gchar * fd_dict_get_answer(const gchar *words)
{
	gchar *answer = NULL;
	gchar *tmp_words;
	gchar *cwords;
	int len;
	int i;

	tmp_words = g_strstrip(words);
	len = strlen(tmp_words);

	/* remove leading evil chars */
	for (i = 0; i < len; i++) {
		if (g_ascii_ispunct(tmp_words[i]) || !g_ascii_isgraph(tmp_words[i]))
			tmp_words[i] = 0;
		else
			break;
	}

	cwords = &tmp_words[i];
	len = strlen(cwords);

	/* remove trailling evil chars */
	for (i = len - 1; i >= 0; i--) {
		if (g_ascii_ispunct(cwords[i]) || !g_ascii_isgraph(cwords[i]))
			cwords[i] = 0;
		else
			break;
	}

	if (cwords[0])
		answer = _fd_dict_get_answer(cwords);
	else
		answer = g_strdup("Not Found!");

	g_free(tmp_words);

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


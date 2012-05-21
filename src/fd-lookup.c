#include <glib.h>

#include "fd-common.h"
#include "fd-dict.h"
#include "fd-str.h"
#include "fd-iciba.h"


void fd_lookup_context_init(struct fd_lookup_context *ctx,
		const gchar *words, const gchar *context)
{
	ctx->words = g_strdup(words);
	ctx->context = g_strdup(context);
}

static void user_dict_record_destroy(gpointer data)
{
	struct fd_user_dict_record *urec = data;

	fd_user_dict_record_free(urec);
	g_free(urec);
}

static void dict_record_destroy(gpointer data)
{
	struct fd_dict_record *rec = data;

	g_free(rec->words);
	g_free(rec->answer);
	g_free(rec);
}

void fd_lookup_context_destroy(struct fd_lookup_context *ctx)
{
	if (ctx->words)
		g_free(ctx->words);
	if (ctx->context)
		g_free(ctx->context);
	if (ctx->build_context)
		g_free(ctx->build_context);
	if (ctx->result_answer)
		g_free(ctx->result_answer);
	if (ctx->dict_answers)
		g_list_free_full(ctx->dict_answers, dict_record_destroy);
	if (ctx->user_dict_answers)
		g_list_free_full(ctx->user_dict_answers, user_dict_record_destroy);

	memset(ctx, 0, sizeof(*ctx));
}

static void user_dict_build_func(gpointer data, gpointer user_data)
{
	struct fd_lookup_context *ctx = user_data;
	struct fd_user_dict_record *urec = data;
	gchar *new_result;

	new_result = g_strconcat(ctx->result_answer ? ctx->result_answer : "",
			urec->Answer, NULL);
	ctx->result_answer = new_result;
}

static void build_func(gpointer data, gpointer user_data)
{
	struct fd_lookup_context *ctx = user_data;
	struct fd_dict_record *rec = data;
	gchar *new_result;

	/*
	 * prepend target key words
	 */
	if (strcmp(ctx->words, rec->words))
		ctx->result_answer = g_strconcat(ctx->result_answer ? ctx->result_answer : "",
			"<", rec->words, ">\n", NULL);

	new_result = g_strconcat(ctx->result_answer ? ctx->result_answer : "",
			rec->answer, NULL);
	ctx->result_answer = new_result;
}

gchar * fd_lookup_context_build_answer(struct fd_lookup_context *ctx)
{
	if (!ctx->result_answer) {
		if (g_list_length(ctx->user_dict_answers)) {
			g_list_foreach(ctx->user_dict_answers, user_dict_build_func, ctx);
			return ctx->result_answer;
		}

		if (g_list_length(ctx->dict_answers) == 0) {
			ctx->result_answer = g_strdup("Not Found!");
		} else {
			g_list_foreach(ctx->dict_answers, build_func, ctx);
		}
	}

	return ctx->result_answer;
}

const gchar * fd_lookup_context_build_context(struct fd_lookup_context *ctx)
{
	if (ctx->build_context)
		return ctx->build_context;

	if (g_list_length(ctx->user_dict_answers)) {
		struct fd_user_dict_record *urec;
		GList *first = g_list_first(ctx->user_dict_answers);
		urec = (struct fd_user_dict_record *)first->data;
		ctx->build_context = g_strconcat("@ ", ctime(&urec->Time), urec->Context, NULL);
	} else {
		ctx->build_context = g_strdup(ctx->context);
	}

	return ctx->build_context;
}

static gboolean lookup(struct fd_lookup_context *lookup_ctx, gchar *cwords)
{
	gchar *answer;
	struct fd_user_dict_record urec;

	/*
	 * first looking in user dict
	 */
	if (fd_user_dict_lookup(cwords, &urec)) {
		struct fd_user_dict_record *new_urec = g_malloc(sizeof(urec));
		*new_urec = urec;
		lookup_ctx->user_dict_answers = g_list_append(
				lookup_ctx->user_dict_answers, new_urec);
		return TRUE;
	}

	/*
	 * at last looking in dict
	 */
	answer = fd_dict_get_answer(cwords);
	if (answer) {
		struct fd_dict_record *rec = g_malloc0(sizeof(rec));
		rec->dict = NULL;
		rec->words = g_strdup(cwords);
		rec->answer = answer;
		lookup_ctx->dict_answers = g_list_append(lookup_ctx->dict_answers, rec);
		return TRUE;
	}

	return FALSE;
}

static gchar * do_lookup(struct fd_loookup_context *lookup_ctx,
		gchar *words)
{
	gboolean got = FALSE;

	got = lookup(lookup_ctx, words);
	if (!got) {
		int words_len = strlen(words);
		gchar *tmp_words = words;
		int suffix_idx = -1;

		do {
			switch (suffix_idx) {
			case -1:
				if (g_str_has_suffix(tmp_words, "ies")) {
					tmp_words[words_len - 2] = 0;
					tmp_words[words_len - 3] = 'y';
					got = lookup(lookup_ctx, tmp_words);
					tmp_words[words_len - 2] = 'e';
					tmp_words[words_len - 3] = 'i';
				}
				break;
			case 0:
				if (g_str_has_suffix(tmp_words, "s")) {
					tmp_words[words_len - 1] = 0;
					got = lookup(lookup_ctx, tmp_words);
					/* continue search the answer from here! */
					if (!got)
						got = lookup(lookup_ctx, tmp_words);
					tmp_words[words_len - 1] = 's';
				}
				break;
			case 1:
				if (g_str_has_suffix(tmp_words, "es")) {
					tmp_words[words_len - 1] = 0;
					tmp_words[words_len - 2] = 0;
					got = lookup(lookup_ctx, tmp_words);
					tmp_words[words_len - 1] = 's';
					tmp_words[words_len - 2] = 'e';
				}
				break;
			case 2:
				if (g_str_has_suffix(tmp_words, "er")) {
					tmp_words[words_len - 1] = 0;
					tmp_words[words_len - 2] = 0;
					got = lookup(lookup_ctx, tmp_words);
					tmp_words[words_len - 1] = 'r';
					tmp_words[words_len - 2] = 'e';
				}
				break;
			case 3:
				if (g_str_has_suffix(tmp_words, "ing")) {
					tmp_words[words_len - 1] = 0;
					tmp_words[words_len - 2] = 0;
					tmp_words[words_len - 3] = 0;
					got = lookup(lookup_ctx, tmp_words);
					if (!got) {
						tmp_words[words_len - 3] = 'e';
						got = do_lookup(lookup_ctx, tmp_words);
					}
					/* search for style: running */
					if (!got && (words_len > 6) &&
						tmp_words[words_len - 4] == tmp_words[words_len - 5]) {
						tmp_words[words_len - 4] = 0;
						got = lookup(lookup_ctx, tmp_words);
						tmp_words[words_len - 4] = tmp_words[words_len - 5];
					}
					tmp_words[words_len - 1] = 'g';
					tmp_words[words_len - 2] = 'n';
					tmp_words[words_len - 3] = 'i';
				}
				break;
			case 4:
				if (g_str_has_suffix(tmp_words, "ed")) {
					tmp_words[words_len - 1] = 0;
					got = lookup(lookup_ctx, tmp_words);
					tmp_words[words_len - 1] = 'd';
					if (!got) {
						tmp_words[words_len - 2] = 0;
						got = lookup(lookup_ctx, tmp_words);
						tmp_words[words_len - 2] = 'e';
					}
					/* search for style: expelled */
					if (!got && (words_len > 4) &&
						tmp_words[words_len - 3] == tmp_words[words_len - 4]) {
						tmp_words[words_len - 3] = 0;
						got = lookup(lookup_ctx, tmp_words);
						tmp_words[words_len - 3] = tmp_words[words_len - 4];
					}
				}
				break;
			case 5:
				if (g_str_has_suffix(tmp_words, "ly")) {
					tmp_words[words_len - 2] = 0;
					got = lookup(lookup_ctx, tmp_words);
					tmp_words[words_len - 2] = 'l';
				}
				break;
			case 6:
				if (g_str_has_suffix(tmp_words, "or")) {
					tmp_words[words_len - 2] = 0;
					got = lookup(lookup_ctx, tmp_words);
					tmp_words[words_len - 2] = 'o';
				}
				break;
			case 7:
				if (g_str_has_suffix(tmp_words, "ness")) {
					tmp_words[words_len - 4] = 0;
					got = do_lookup(lookup_ctx, tmp_words);
					tmp_words[words_len - 4] = 'n';
				}
				break;
			default:
				goto done;
				break;
			}
			suffix_idx++;
		} while (!got);
done:

		/*
		 * check prefix
		 */
		if (!got) {
			int prefix_idx = -1;

			do {
				switch (prefix_idx) {
				case -1:
					if (g_str_has_prefix(tmp_words, "un")) {
						got = do_lookup(lookup_ctx, tmp_words + 2);
					}
					break;
				default:
					goto prefix_check_done;
					break;
				}
				prefix_idx++;
			} while (!got);
prefix_check_done:
			;
		}
	}

	return got;
}

gboolean fd_lookup_exec(struct fd_lookup_context *lookup_ctx)
{
	gchar *dup_words;
	gboolean got = FALSE;
	gchar *tmp_words;
	gchar *cwords;
	int len;
	int i;
	GTimer *timer = NULL;

	if (!timer)
		timer = g_timer_new();


	g_timer_start(timer);

	/*
	 * NOTE: g_strstrip() don't create new one!
	 */
	dup_words = g_strdup(lookup_ctx->words);
	tmp_words = g_strstrip(dup_words);
	len = strlen(tmp_words);

	/* remove leading evil chars */
	for (i = 0; i < len; i++) {
		if (tmp_words[i] & 0x80)
			break;
		if (g_ascii_ispunct(tmp_words[i]) || !g_ascii_isgraph(tmp_words[i]))
			tmp_words[i] = 0;
		else
			break;
	}

	cwords = &tmp_words[i];
	len = strlen(cwords);

	/* remove trailling evil chars */
	for (i = len - 1; i >= 0; i--) {
		if (tmp_words[i] & 0x80)
			break;
		if (g_ascii_ispunct(cwords[i]) || !g_ascii_isgraph(cwords[i]))
			cwords[i] = 0;
		else
			break;
	}

	if (cwords[0]) {
		int feature = fd_str_get_feature(cwords);

		got = do_lookup(lookup_ctx, cwords);
		/*
		 * continue to lookup through strdown and then strup
		 */
		if (!got) {
			gchar *w;
			if (feature != FD_STR_FEATURE_ALL_LOWER) {
				w = g_ascii_strdown(cwords, -1);
				got = do_lookup(lookup_ctx, w);
				g_free(w);
			}
			if (!got && (feature != FD_STR_FEATURE_ALL_UPPER)) {
				/* strup should direct call the dict */
				w = g_ascii_strup(cwords, -1);
				got = lookup(lookup_ctx, w);
				g_free(w);
			}

			/*
			 * search for Camel style words
			 */
			if (!got) {
				w = g_ascii_strdown(cwords, -1);
				w[0] = g_ascii_toupper(w[0]);
				if (!g_str_equal(w, cwords))
					got = do_lookup(lookup_ctx, w);
				g_free(w);
			}
		}

		/*
		 * at last got from online dict
		 */
		if (!got) {
			char *online_answer = fd_iciba_lookup(cwords);
			if (online_answer)
				lookup_ctx->result_answer = g_strdup(online_answer);
		}
	}

	g_free(dup_words);

	g_timer_stop(timer);

	printf("lookup \"%s\" take %f sec.\n",
		lookup_ctx->words, g_timer_elapsed(timer, NULL));

	return TRUE;
}


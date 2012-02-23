#include <glib.h>

#include "fd-common.h"
#include "fd-dict.h"


void fd_lookup_context_init(struct fd_lookup_context *ctx, const gchar *words)
{
	ctx->words = words;
}

void fd_lookup_context_destroy(struct fd_lookup_context *ctx)
{
	if (ctx->result_answer)
		g_free(ctx->result_answer);
	if (ctx->dict_answers)
		g_list_free(ctx->dict_answers);
	if (ctx->user_dict_answers)
		g_list_free(ctx->user_dict_answers);
}

gchar * fd_lookup_context_build_answer(struct fd_lookup_context *ctx)
{
	return ctx->result_answer;
}

static gchar * do_lookup(struct fd_loookup_context *lookup_ctx,
		gchar *words)
{
	gchar *answer;

	answer = fd_dict_get_answer(words);
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
					answer = fd_dict_get_answer(tmp_words);
					tmp_words[words_len - 2] = 'e';
					tmp_words[words_len - 3] = 'i';
				}
				break;
			case 0:
				if (g_str_has_suffix(tmp_words, "s")) {
					tmp_words[words_len - 1] = 0;
					answer = fd_dict_get_answer(tmp_words);
					/* continue search the answer from here! */
					if (!answer)
						 answer = do_lookup(lookup_ctx, tmp_words);
					tmp_words[words_len - 1] = 's';
				}
				break;
			case 1:
				if (g_str_has_suffix(tmp_words, "es")) {
					tmp_words[words_len - 1] = 0;
					tmp_words[words_len - 2] = 0;
					answer = fd_dict_get_answer(tmp_words);
					tmp_words[words_len - 1] = 's';
					tmp_words[words_len - 2] = 'e';
				}
				break;
			case 2:
				if (g_str_has_suffix(tmp_words, "er")) {
					tmp_words[words_len - 1] = 0;
					tmp_words[words_len - 2] = 0;
					answer = fd_dict_get_answer(tmp_words);
					tmp_words[words_len - 1] = 'r';
					tmp_words[words_len - 2] = 'e';
				}
				break;
			case 3:
				if (g_str_has_suffix(tmp_words, "ing")) {
					tmp_words[words_len - 1] = 0;
					tmp_words[words_len - 2] = 0;
					tmp_words[words_len - 3] = 0;
					answer = fd_dict_get_answer(tmp_words);
					tmp_words[words_len - 1] = 'g';
					tmp_words[words_len - 2] = 'n';
					tmp_words[words_len - 3] = 'i';
				}
				break;
			case 4:
				if (g_str_has_suffix(tmp_words, "ed")) {
					tmp_words[words_len - 1] = 0;
					answer = fd_dict_get_answer(tmp_words);
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

gboolean fd_lookup_exec(struct fd_lookup_context *lookup_ctx)
{
	gchar *dup_words;
	gchar *answer = NULL;
	gchar *tmp_words;
	gchar *cwords;
	int len;
	int i;

	/*
	 * NOTE: g_strstrip() don't create new one!
	 */
	dup_words = g_strdup(lookup_ctx->words);
	tmp_words = g_strstrip(dup_words);
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
		answer = do_lookup(lookup_ctx, cwords);
	else
		answer = g_strdup("Not Found!");

	g_free(dup_words);

	lookup_ctx->result_answer = answer;

	return TRUE;
}


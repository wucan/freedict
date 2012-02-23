#include <glib.h>

#include "fd-common.h"


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


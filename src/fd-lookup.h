#ifndef _FD_LOOKUP_H_
#define _FD_LOOKUP_H_

#include "fd-common.h"


void fd_lookup_context_init(struct fd_lookup_context *ctx, const gchar *words);
void fd_lookup_context_destroy(struct fd_lookup_context *ctx);
gchar * fd_lookup_context_build_answer(struct fd_lookup_context *ctx);

gboolean fd_lookup_exec(struct fd_lookup_context *lookup_ctx);


#endif /* _FD_LOOKUP_H_ */


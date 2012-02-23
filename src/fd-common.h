#ifndef _FD_COMMON_H_
#define _FD_COMMON_H_

#include <glib/glist.h>


struct fd_dict {
	gchar *uri;
	gchar *char_encode;
	gchar *data;
	gsize size;

	gchar * (*get_answer)(const gchar *words);
};

struct fd_user_dict_record {
	int Count;
};

struct fd_dict_record {
	struct fd_dict *dict;

	gchar *words;
	gchar *answer;
};

struct fd_lookup_context {
	/* input data */
	gchar *words;

	gchar *result_answer;

	/* answer from dict, list of fd_dict_record */
	GList *dict_answers;

	/* answer from user_dict, list of fd_user_dict_record */
	GList *user_dict_answers;
};


#endif /* _FD_COMMON_H_ */


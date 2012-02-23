#ifndef _FD_COMMON_H_
#define _FD_COMMON_H_


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

struct fd_lookup_context {
	/* input data */
	gchar *words;

	gchar *result_answer;
};


#endif /* _FD_COMMON_H_ */


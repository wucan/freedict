#ifndef _FD_USER_DICT_H_
#define _FD_USER_DICT_H_


int fd_user_dict_open();
void fd_user_dict_close();
gboolean fd_user_dict_lookup(gchar *word, struct fd_user_dict_record *r);
int fd_user_dict_add(gchar *word, gchar *answer, gchar *context);


#endif /* _FD_USER_DICT_H_ */


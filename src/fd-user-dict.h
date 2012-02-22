#ifndef _FD_USER_DICT_H_
#define _FD_USER_DICT_H_


int fd_user_dict_open();
void fd_user_dict_close();
int fd_user_dict_add(gchar *word, gchar *answer, gchar *context);


#endif /* _FD_USER_DICT_H_ */


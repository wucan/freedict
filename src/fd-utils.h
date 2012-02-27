#ifndef _FD_UTILS_H_
#define _FD_UTILS_H_


gboolean fd_utils_init();
void fd_utils_deinit();
gchar * fd_utils_get_active_window_title();
int x11_mouse_position(int *x, int *y);
int gdk_get_mouse_position(int *x, int *y);
gboolean is_mouse_nearby_stage_window(GtkWidget *stage);
void hex_dump(const gchar *prefix, guchar *buf, int size);


#endif /* _FD_UTILS_H_ */


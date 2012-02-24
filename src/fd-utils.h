#ifndef _FD_UTILS_H_
#define _FD_UTILS_H_


gchar * fd_utils_get_active_window_title();
int x11_mouse_position(int *x, int *y);
int gdk_get_mouse_position(int *x, int *y);
gboolean is_mouse_nearby_stage_window(GtkWidget *stage);


#endif /* _FD_UTILS_H_ */


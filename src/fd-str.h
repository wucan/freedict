#ifndef _FD_STR_H_
#define _FD_STR_H_


/*
 * string feature: all lower, all upper, mix
 */
#define FD_STR_FEATURE_ALL_LOWER		(1 << 0)
#define FD_STR_FEATURE_ALL_UPPER		(1 << 1)
#define FD_STR_FEATURE_MIX				((1 << 0) | (1 << 1))

gboolean fd_str_is_all_upper(gchar *s);
gboolean fd_str_is_all_lower(gchar *s);
gboolean fd_str_is_mix(gchar *s);
gint fd_str_get_feature(gchar *s);

gboolean fd_ascii_is_vowel(gchar c);


#endif /* _FD_STR_H_ */


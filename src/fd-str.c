#include <glib.h>

#include "fd-str.h"


gboolean fd_str_is_all_upper(gchar *s)
{
	while (*s) {
		if (g_ascii_islower(*s))
			return FALSE;
		s++;
	}

	return TRUE;
}

gboolean fd_str_is_all_lower(gchar *s)
{
	while (*s) {
		if (g_ascii_isupper(*s))
			return FALSE;
		s++;
	}

	return TRUE;
}

gint fd_str_get_feature(gchar *s)
{
	int check_flags = 0;

	while (*s) {
		if (g_ascii_islower(*s))
			check_flags |= (1 << 0);
		if (g_ascii_isupper(*s))
			check_flags |= (1 << 1);
		if (check_flags == (1 << 1) | (1 << 0))
			break;
		s++;
	}

	return check_flags;
}

gboolean fd_str_is_mix(gchar *s)
{
	return (fd_str_get_feature(s) == FD_STR_FEATURE_MIX);
}

gboolean fd_ascii_is_vowel(gchar c)
{
	if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
		return TRUE;
	return FALSE;
}


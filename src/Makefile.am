bin_PROGRAMS=freedict
freedict_SOURCES=main.c fd-systray.c fd-systray.h \
	fd-window.c fd-window.h \
	gtk_support.c gtk_support.h gdk_support.c gdk_support.h \
	fd-stage.c fd-clipboard.c fd-dict.c fd-user-dict.c fd-lookup.c \
	fd-str.c fd-utils.c fd-iciba.c

freedict_CFLAGS=\
	-I$(top_srcdir) -I$(top_builddir) \
	$(GTK_CFLAGS) $(SQLITE3_CFLAGS) $(LIBXML2_CFLAGS)

freedict_CFLAGS += -g

#  uncomment the following if freedict requires the math library
freedict_LDADD=$(GTK_LIBS) $(SQLITE3_LIBS) $(LIBXML2_LIBS) -lX11

EXTRA_DIST=SConscript


bin_PROGRAMS=freedict
freedict_SOURCES=main.c fd-systray.c fd-systray.h \
	fd-window.c fd-window.h \
	gtk_support.c gtk_support.h gdk_support.c gdk_support.h

freedict_CFLAGS=\
	-I$(top_srcdir) -I$(top_builddir) \
	$(GTK_CFLAGS) $(GNOME_CFLAGS)

#  uncomment the following if freedict requires the math library
freedict_LDADD=$(GTK_LIBS) $(GNOME_LIBS)

EXTRA_DIST=SConscript


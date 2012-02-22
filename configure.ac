dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.57])
AC_INIT([Freedict], [0.01], [http://xxx], [freedict])
dnl Every other copy of the package version number gets its value from here
AM_INIT_AUTOMAKE(freedict, 0.01)

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])

AC_SUBST(VERSION)

dnl Checks for programs.
AM_PROG_LIBTOOL

AC_PROG_INSTALL
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_STDC
dnl enable *.c per-target flags in Makefile.am
AM_PROG_CC_C_O

AC_STDC_HEADERS

dnl Checks for libraries.
dnl Specify required versions of dependencies
GLIB_REQUIRED=2.8.0
GTK_REQUIRED=2.14.0
AC_SUBST([GLIB_REQUIRED])
AC_SUBST([GTK_REQUIRED])

dnl Check dependencies
PKG_CHECK_MODULES(GTK, gtk+-2.0 >= $GTK_REQUIRED gthread-2.0 gio-2.0 >= $GLIB_REQUIRED gdk-pixbuf-2.0)
PKG_CHECK_MODULES(GNOME, libgnome-2.0 libgnomeui-2.0)
PKG_CHECK_MODULES(SQLITE3, sqlite3)

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.

AC_OUTPUT(Makefile src/Makefile)

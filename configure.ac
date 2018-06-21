# Initialization

AC_INIT([De-Interleaver], [1], [hagjan@protonmail.com])
AC_CONFIG_SRCDIR([src/gui.c])
AM_INIT_AUTOMAKE([-Wall foreign])
AM_SILENT_RULES([yes])

# Shopping list
AC_PROG_CC
PKG_PROG_PKG_CONFIG

# Libraries
PKG_CHECK_MODULES([DE_INTERLEAVER], [
	glib-2.0
	gtk+-3.0
	sndfile
])

# Output
AC_CONFIG_FILES([
    Makefile
    src/Makefile
    pixmaps/Makefile
])
AC_OUTPUT


dnl This is not shown!

ACLOCAL_AMFLAGS= -I m4

AM_CFLAGS = -I$(top_srcdir)/include -Wimplicit-function-declaration -Wformat

lib_LTLIBRARIES = libgloss-g3dexport.la
libgloss_g3dexport_la_SOURCES = $(LIBGLOSSG3DEXPORT_C_FILES)

LIBGLOSSG3DEXPORT_C_FILES = g3dexport.c \
                            stl.c \
                            obj.c

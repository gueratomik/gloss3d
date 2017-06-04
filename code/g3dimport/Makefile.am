ACLOCAL_AMFLAGS= -I m4

AM_CFLAGS = -I$(top_srcdir)/include -Wimplicit-function-declaration -Wformat

lib_LTLIBRARIES = libgloss-g3dimport.la
libgloss_g3dimport_la_SOURCES = $(LIBGLOSSG3DIMPORT_C_FILES)

LIBGLOSSG3DIMPORT_C_FILES = g3dimport.c \
                            g3dimportextension.c \
                            collada.c \
                            3ds.c \
                            obj.c \
                            # c4d.c


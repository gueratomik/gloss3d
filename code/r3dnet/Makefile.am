ACLOCAL_AMFLAGS= -I m4

AM_CFLAGS  = -I$(top_srcdir)/include -Wimplicit-function-declaration -Wformat

lib_LTLIBRARIES = libgloss-r3dnet.la
libgloss_r3dnet_la_SOURCES = $(LIBGLOSSR3DNET_C_FILES)

LIBGLOSSR3DNET_C_FILES = r3dnetclient.c \
                         r3dnetframe.c \
                         r3dnetserver.c

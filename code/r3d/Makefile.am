ACLOCAL_AMFLAGS= -I m4

AM_CFLAGS = -I$(top_srcdir)/include -Wimplicit-function-declaration -Wformat

lib_LTLIBRARIES = libgloss-r3d.la
libgloss_r3d_la_SOURCES = $(LIBGLOSSR3D_C_FILES)

LIBGLOSSR3D_C_FILES = r3darea.c \
                      r3d.c \
                      r3dcamera.c \
                      r3dface.c \
                      r3dinterpolation.c \
                      r3dlight.c \
                      r3dmesh.c \
                      r3dobject.c \
                      r3doctree.c \
                      r3dplane.c \
                      r3dray.c \
                      r3dscene.c \
                      r3dfilter.c

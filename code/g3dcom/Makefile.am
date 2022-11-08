ACLOCAL_AMFLAGS= -I m4

if HAVE_WIN32
    AM_LDFLAGS = -shared -no-undefined
endif

lib_LTLIBRARIES = libgloss-g3dcom.la
libgloss_g3dcom_la_SOURCES = $(LIBGLOSSG3DCOM_C_FILES)

LIBGLOSSG3DCOM_C_FILES = g3dcom.c

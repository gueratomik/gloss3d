ACLOCAL_AMFLAGS= -I m4

AM_CFLAGS = -I$(top_srcdir)/include -Wimplicit-function-declaration -Wformat

lib_LTLIBRARIES = libgloss-g3dmouse.la
libgloss_g3dmouse_la_SOURCES = $(LIBGLOSSG3DMOUSE_C_FILES)

LIBGLOSSG3DMOUSE_C_FILES = createCube.c     \
                           createCylinder.c \
                           createFace.c     \
                           createPlane.c     \
                           createSphere.c   \
                           createVertex.c   \
                           createBone.c     \
                           createTorus.c    \
                           cutMesh.c        \
                           triangulate.c    \
                           untriangulate.c  \
                           paintWeight.c    \
                           invertNormal.c    \
                           empty.c          \
                           extrudeFace.c    \
                           g3dmousetool.c   \
                           weldvertices.c   \
                           move.c           \
                           pick.c           \
                           rotate.c         \
                           scale.c


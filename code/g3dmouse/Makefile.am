ACLOCAL_AMFLAGS= -I m4

if HAVE_WIN32
    AM_LDFLAGS = -shared -no-undefined -L../g3durmanager/.libs/ -lgloss-g3durmanager \
                                       -L../makeup3d/.libs/ -lgloss-makeup3d         \
                                       -L../g3dengine/.libs/ -lgloss-g3dengine       \
                                       -L../list/.libs/ -lgloss-list
endif

lib_LTLIBRARIES = libgloss-g3dmouse.la
libgloss_g3dmouse_la_SOURCES = $(LIBGLOSSG3DMOUSE_C_FILES)

LIBGLOSSG3DMOUSE_C_FILES = g3dmousetool.c             \
                           gloss3d/createCube.c       \
                           gloss3d/createCylinder.c   \
                           gloss3d/bridge.c           \
                           gloss3d/createPlane.c      \
                           gloss3d/createSphere.c     \
                           gloss3d/createVertex.c     \
                           gloss3d/createBone.c       \
                           gloss3d/createTorus.c      \
                           gloss3d/createTube.c       \
                           gloss3d/cutMesh.c          \
                           gloss3d/createFacegroup.c  \
                           gloss3d/triangulate.c      \
                           gloss3d/untriangulate.c    \
                           gloss3d/makeeditable.c    \
                           gloss3d/revertSpline.c     \
                           gloss3d/removeVertexPose.c \
                           gloss3d/roundSplinePoint.c \
                           gloss3d/invertNormal.c     \
                           gloss3d/empty.c            \
                           gloss3d/extrudeFace.c      \
                           gloss3d/weldvertices.c     \
                           gloss3d/move.c             \
                           gloss3d/pick.c             \
                           gloss3d/rotate.c           \
                           gloss3d/sculpt.c           \
                           gloss3d/scale.c            \
                           makeup3d/spray.c


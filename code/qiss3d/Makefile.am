ACLOCAL_AMFLAGS= -I m4

if HAVE_WIN32
    AM_LDFLAGS = -shared -no-undefined -L../g3dimport/.libs/ -lgloss-g3dimport \
                                       -L../g3dexport/.libs/ -lgloss-g3dexport \
                                       -L../g3dengine/.libs/ -lgloss-g3dengine \
                                       -L../list/.libs/ -lgloss-list \
                                       -lavifil32 -lgdi32
endif

lib_LTLIBRARIES = libgloss-q3d.la
libgloss_q3d_la_SOURCES = $(LIBGLOSSQ3D_C_FILES)

LIBGLOSSQ3D_C_FILES = q3d.c \
                      q3dsettings.c \
                      q3dbounding.c \
                      q3dinstance.c \
                      q3dsymmetry.c \
                      q3doctree.c \
                      q3dvertexset.c \
                      q3dmesh.c \
                      q3dsphere.c \
                      q3dzengine.c \
                      q3dplane.c \
                      q3dobject.c \
                      q3dscene.c \
                      q3darea.c \
                      q3djob.c \
                      q3dfilter.c \
                      filters/towindow.unix.c \
                      filters/towindow.win32.c \
                      filters/simpleaa.c \
                      filters/writeimage.c \
                      filters/toffmpeg.c \
                      filters/softshadows.c \
                      filters/vectormotionblur.c \
                      filters/scenemotionblur.c \
                      filters/edgeaa.c \
                      q3dlight.c \
                      q3dvector.c \
                      q3dinterpolation.c \
                      q3dcamera.c \
                      q3dparticleemitter.c \
                      q3dray.c \
                      q3dtriangle.c

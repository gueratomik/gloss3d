ACLOCAL_AMFLAGS= -I m4

if HAVE_WIN32
    AM_LDFLAGS = -shared -no-undefined -L../g3dengine/.libs/ -lgloss-g3dengine -L../list/.libs/ -lgloss-list
endif

lib_LTLIBRARIES = libgloss-g3dexport.la
libgloss_g3dexport_la_SOURCES = $(LIBGLOSSG3DEXPORT_C_FILES)

LIBGLOSSG3DEXPORT_C_FILES = v2/g3dexportextension.c \
                            v2/g3dexportapp.c \
                            v2/g3dexportbone.c \
                            v2/g3dexport.c \
                            v2/g3dexportcamera.c \
                            v2/g3dexportchannel.c \
                            v2/g3dexportlight.c \
                            v2/g3dexportmaterial.c \
                            v2/g3dexportmesh.c \
                            v2/g3dexportmodifiers.c \
                            v2/g3dexportobject.c \
                            v2/g3dexportprimitive.c \
                            v2/g3dexportuvmap.c \
                            v2/g3dexportscene.c \
                            v2/g3dexportsymmetry.c \
                            v2/g3dexportsubdivider.c \
                            v2/g3dexportspline.c \
                            v2/g3dexportsplinerevolver.c \
                            v2/g3dexporttext.c \
                            v2/g3dexportwireframe.c \
                            v2/g3dexportffd.c \
                            v2/g3dexportskin.c \
                            v2/g3dexportkey.c \
                            v2/g3dexportmorpher.c \
                            v3/g3dexportextension.c \
                            v3/g3dexportapp.c \
                            v3/g3dexportbone.c \
                            v3/g3dexport.c \
                            v3/g3dexportcamera.c \
                            v3/g3dexportchannel.c \
                            v3/g3dexportinstance.c \
                            v3/g3dexportlight.c \
                            v3/g3dexportmaterial.c \
                            v3/g3dexportmesh.c \
                            v3/g3dexportmodifiers.c \
                            v3/g3dexportobject.c \
                            v3/g3dexportparticleemitter.c \
                            v3/g3dexportprimitive.c \
                            v3/g3dexportuvmap.c \
                            v3/g3dexportscene.c \
                            v3/g3dexportsymmetry.c \
                            v3/g3dexportsubdivider.c \
                            v3/g3dexportspline.c \
                            v3/g3dexportsplinerevolver.c \
                            v3/g3dexporttext.c \
                            v3/g3dexportwireframe.c \
                            v3/g3dexportffd.c \
                            v3/g3dexportskin.c \
                            v3/g3dexportkey.c \
                            v3/g3dexporttag.c \
                            v3/g3dexportmorpher.c \
                            stl.c \
                            pov.c \
                            obj.c

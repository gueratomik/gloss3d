ACLOCAL_AMFLAGS= -I m4

if HAVE_WIN32
    AM_LDFLAGS = -L../list/.libs/ -shared -no-undefined `pkg-config --cflags glib-2.0` -lglib-2.0 -lopengl32 -lglu32 -lglew32 -ljpeg -lgloss-list
endif

lib_LTLIBRARIES = libgloss-g3dengine.la
libgloss_g3dengine_la_SOURCES = $(LIBGLOSSG3D_C_FILES)

##include_HEADERS = $(LIBGLOSSG3D_H_FILES)

LIBGLOSSG3D_C_FILES = g2dvector.c \
                      g3dbbox.c \
                      g3dbone.c \
                      g3dcamera.c \
                      g3dchannel.c \
                      g3dcore.c \
                      g3dcube.c \
                      g3dcursor.c \
                      g3dcurve.c \
                      g3dcutface.c \
                      g3dcylinder.c \
                      g3dedge.c \
                      g3dengine.c \
                      g3dextension.c \
                      g3dface.c \
                      g3dfacegroup.c \
                      g3dffd.c \
                      g3dheightmap.c \
                      g3dinstance.c \
                      g3dimage.c \
                      g3dkey.c \
                      g3dlight.c \
                      g3dlookup.c \
                      g3dmaterial.c \
                      g3dmesh.c \
                      g3dmorpher.c \
                      g3dmodifier.c \
                      g3dobject.c \
                      g3dpick.c \
                      g3dpivot.c \
                      g3dplane.c \
                      g3dparticleemitter.c \
                      g3dprimitive.c \
                      g3dprocedural.c \
                      g3dproceduralbrick.c \
                      g3dproceduralchessboard.c \
                      g3dproceduralgradient.c \
                      g3dproceduralnoise.c \
                      g3dquaternion.c \
                      g3drgba.c \
                      g3drig.c \
                      g3drttriangle.c \
                      g3drttriangleuvw.c \
                      shadervertex.c \
                      g3dscene.c \
                      g3dskin.c \
                      g3dsphere.c \
                      g3dspline.c \
                      g3dsplinerevolver.c \
                      g3dsubdivisionthread.c \
                      g3dsubdivider.c \
                      g3dsubdivisionV3.c \
                      g3dsubedge.c \
                      g3dsubface.c \
                      g3dsubindex.c \
                      g3dsubvertex.c \
                      g3dsymmetry.c \
                      g3dsysinfo.c \
                      g3dtag.c \
                      g3dtext.c \
                      g3dtexture.c \
                      g3dtorus.c \
                      g3dtube.c \
                      g3duv.c \
                      g3dvector.c \
                      g3dvertex.c \
                      g3dweight.c \
                      g3dwireframe.c

##LIBGLOSSG3D_H_FILES = include/g3d.h include/list.h

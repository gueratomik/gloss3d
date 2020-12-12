ACLOCAL_AMFLAGS= -I m4


if HAVE_WIN32
    AM_LDFLAGS = -Wl,--no-as-needed -lGL -lGLU -ljpeg -lpng -lm
    AM_CFLAGS  = -g -I$(top_srcdir)/include -I/usr/src/tarball/libjpg-win32/include -Wimplicit-function-declaration -Wformat 
endif

if HAVE_GTK3
    AM_LDFLAGS = -Wl,--no-as-needed -lXpm -lGL -lGLU -lX11 `pkg-config --libs freetype2` -ljpeg -lpng -lm -lpthread -lc
    AM_CFLAGS  = -g -I$(top_srcdir)/include -L/usr/lib -L/usr/lib/i386-linux-gnu -L/usr/lib/x86_64-linux-gnu `pkg-config --cflags freetype2` -Wimplicit-function-declaration -Wformat 
endif

if HAVE_MOTIF
    AM_LDFLAGS = -Wl,--no-as-needed -lXpm -lGL -lGLU -lGLw -lXt -lX11 `pkg-config --libs freetype2` -ljpeg -lpng -lm -lpthread -lc
    AM_CFLAGS  = -g -I$(top_srcdir)/include -L/usr/lib -L/usr/lib/i386-linux-gnu -L/usr/lib/x86_64-linux-gnu `pkg-config --cflags freetype2` -Wimplicit-function-declaration -Wformat 
endif

SUBDIRS = g3dengine \
          g3dexport \
          g3dimport \
          g3dmouse \
          g3dcom \
          g3dui \
          g3durmanager \
          lips3d \
          r3d
#          r3dnet

if HAVE_GTK3
    bin_PROGRAMS = gloss3d-gtk3
    gloss3d_gtk3_SOURCES = $(BINGLOSS3D_C_FILES)

    gloss3d_gtk3_LDADD = g3dengine/libgloss-g3dengine.la \
                         g3dexport/libgloss-g3dexport.la \
                         g3dimport/libgloss-g3dimport.la \
                         g3dmouse/libgloss-g3dmouse.la \
                         g3dcom/libgloss-g3dcom.la \
                         g3dui/libgloss-g3dui-gtk3.la \
                         g3durmanager/libgloss-g3durmanager.la \
                         r3d/libgloss-r3d.la \
                         lips3d/libgloss-lips3d.la
#                         r3dnet/libgloss-r3dnet.la

    BINGLOSS3D_C_FILES   = main.c \
                           list.c
endif

if HAVE_MOTIF
    bin_PROGRAMS = gloss3d-motif
    gloss3d_motif_SOURCES = $(BINGLOSS3D_C_FILES)

    gloss3d_motif_LDADD = g3dengine/libgloss-g3d.la \
                          g3dexport/libgloss-g3dexport.la \
                          g3dimport/libgloss-g3dimport.la \
                          g3dmouse/libgloss-g3dmouse.la \
                          g3dcom/libgloss-g3dcom.la \
                          g3dui/libgloss-g3dui-motif.la \
                          g3durmanager/libgloss-g3durmanager.la \
                          r3d/libgloss-r3d.la \
                          lips3d/libgloss-lips3d.la
#                          r3dnet/libgloss-r3dnet.la

    BINGLOSS3D_C_FILES   = main.c \
                           GLwDrawA.c \
                           GLwMDrawA.c \
                           list.c
endif

if HAVE_WIN32
    bin_PROGRAMS = gloss3d-win32
    gloss3d_win32_SOURCES = $(BINGLOSS3D_C_FILES)

    gloss3d_win32_LDADD = g3dengine/libgloss-g3d.la \
                          g3dexport/libgloss-g3dexport.la \
                          g3dimport/libgloss-g3dimport.la \
                          g3dmouse/libgloss-g3dmouse.la \
                          g3dcom/libgloss-g3dcom.la \
                          g3dui/libgloss-g3dui-gtk3.la \
                          g3durmanager/libgloss-g3durmanager.la \
                          r3d/libgloss-r3d.la
#                          r3dnet/libgloss-r3dnet.la

    BINGLOSS3D_C_FILES   = main.c \
                           list.c
endif

EXTRA_DIST   = include/g3dexport.h \
                       include/g3dexportv2.h \
                       include/g3dengine/g2dvector.h \
                       include/g3dengine/g3dcolor.h \
                       include/g3dengine/g3dcurve.h \
                       include/g3dengine/g3dengine.h \
                       include/g3dengine/g3dmorpher.h \
                       include/g3dengine/g3dimage.h \
                       include/g3dengine/g3dlight.h \
                       include/g3dengine/g3dlookup.h \
                       include/g3dengine/g3dmesh.h \
                       include/g3dengine/g3drgba.h \
                       include/g3dengine/g3dpick.h \
                       include/g3dengine/g3dspline.h \
                       include/g3dengine/g3dsplinerevolver.h \
                       include/g3dengine/g3dsubdivisionthread.h \
                       include/g3dengine/g3dsubdivisionV3.h \
                       include/g3dengine/g3dsysinfo.h \
                       include/g3dengine/g3dtext.h \
                       include/g3dengine/g3dtinyvector.h \
                       include/g3dengine/g3dvector.h \
                       include/lips3d/lips3d.h \
                       include/g3d.h \
                       include/g3dimport.h \
                       include/g3dimportv2.h \
                       include/g3dmouse.h \
                       include/g3dui.h \
                       include/g3dui_motif.h \
                       include/g3dui_gtk3.h \
                       include/g3dcom.h \
                       include/g3durmanager.h \
                       include/list.h \
                       include/r3d.h \
                       include/r3dnet.h \
                       include/TabbedBook.h \
                       include/ColorEdit.h \
                       include/MaterialEdit.h \
                       include/GLwDrawA.h \
                       include/GLwDrawAP.h \
                       include/GLwMDrawA.h \
                       include/GLwMDrawAP.h \
                       include/signatures \
                       include/signatures.h \
                       include/siggen.sh \
                       xpm/addvertex.xpm \
                       xpm/axismode.xpm \
                       xpm/bonico.xpm \
                       xpm/bridge.xpm \
                       xpm/bucket.xpm \
                       xpm/camico.xpm \
                       xpm/cube.xpm \
                       xpm/cubico.xpm \
                       xpm/cylico.xpm \
                       xpm/cylinder.xpm \
                       xpm/defico.xpm \
                       xpm/delete.xpm \
                       xpm/disico.xpm \
                       xpm/edgemode.xpm \
                       xpm/enaico.xpm \
                       xpm/eraser.xpm \
                       xpm/extrude.xpm \
                       xpm/eyeopn.xpm \
                       xpm/eyesht.xpm \
                       xpm/facemode.xpm \
                       xpm/ffdico.xpm \
                       xpm/horz.xpm \
                       xpm/knife.xpm \
                       xpm/ligico.xpm \
                       xpm/makeedit.xpm \
                       xpm/mapico.xpm \
                       xpm/matedit.xpm \
                       xpm/maximize_view.xpm \
                       xpm/mesico.xpm \
                       xpm/move.xpm \
                       xpm/newfile.xpm \
                       xpm/nextframe.xpm \
                       xpm/objectmode.xpm \
                       xpm/openfile.xpm \
                       xpm/pen.xpm \
                       xpm/pick.xpm \
                       xpm/playanim.xpm \
                       xpm/plnico.xpm \
                       xpm/prevframe.xpm \
                       xpm/record.xpm \
                       xpm/redo.xpm \
                       xpm/renderw.xpm \
                       xpm/render.xpm \
                       xpm/rotate_view.xpm \
                       xpm/rotate.xpm \
                       xpm/saveas.xpm \
                       xpm/saveimageas.xpm \
                       xpm/save.xpm \
                       xpm/saveimage.xpm \
                       xpm/scale.xpm \
                       xpm/scnico.xpm \
                       xpm/sculptmode.xpm \
                       xpm/selectarea.xpm \
                       xpm/selectrandom.xpm \
                       xpm/shor.xpm \
                       xpm/skinmode.xpm \
                       xpm/sobj.xpm \
                       xpm/sphere.xpm \
                       xpm/sphico.xpm \
                       xpm/splico.xpm \
                       xpm/srvico.xpm \
                       xpm/stopanim.xpm \
                       xpm/stoprender.xpm \
                       xpm/subico.xpm \
                       xpm/symico.xpm \
                       xpm/torico.xpm \
                       xpm/translate_view.xpm \
                       xpm/tubico.xpm \
                       xpm/txtico.xpm \
                       xpm/undo.xpm \
                       xpm/morpher.xpm \
                       xpm/unselectarea.xpm \
                       xpm/unzoomtime.xpm \
                       xpm/uvwmapmode.xpm \
                       xpm/vertexmode.xpm \
                       xpm/vert.xpm \
                       xpm/wirico.xpm \
                       xpm/xaxis.xpm \
                       xpm/yaxis.xpm \
                       xpm/zaxis.xpm \
                       xpm/zoomtime.xpm \
                       xpm/zoom_view.xpm \
                       xpm/brush_0.c \
                       xpm/brush_1.c \
                       xpm/brush_2.c \
                       xpm/brush_3.c \
                       xpm/brush_4.c \
                       xpm/brush_5.c


backup:
	tar -cvzf ../Motif-`date +%Y%m%d`.tar.gz `find . -type f | grep -v "\.o"` && \
        ./backup.sh ../Motif-`date +%Y%m%d`.tar.gz && \
        rm -f ../Motif-`date +%Y%m%d`.tar.gz
.PHONY: backup

count:
	cloc * --exclude-lang=CSS,Javascript,HTML,m4,make,Bourne\ Shell,Bourne\ Again\ Shell
.PHONY: count

ACLOCAL_AMFLAGS= -I m4


if HAVE_WIN32
    AM_LDFLAGS = -Wl,--no-as-needed -lGL -lGLU -ljpeg -lpng -lm
    AM_CFLAGS  = -g -I$(top_srcdir)/include -I/usr/src/tarball/libjpg-win32/include -Wimplicit-function-declaration -Wformat 
endif

if HAVE_GTK3
    AM_LDFLAGS = -Wl,--no-as-needed -lXpm -lGL -lGLU -lX11 -ljpeg -lpng -lm -lpthread -lc
    AM_CFLAGS  = -g -I$(top_srcdir)/include -L/usr/lib -L/usr/lib/i386-linux-gnu -L/usr/lib/x86_64-linux-gnu -Wimplicit-function-declaration -Wformat 
endif

if HAVE_MOTIF
    AM_LDFLAGS = -Wl,--no-as-needed -lXpm -lGL -lGLU -lGLw -lXt -lX11 -ljpeg -lpng -lm -lpthread -lc
    AM_CFLAGS  = -g -I$(top_srcdir)/include -L/usr/lib -L/usr/lib/i386-linux-gnu -L/usr/lib/x86_64-linux-gnu -Wimplicit-function-declaration -Wformat 
endif

SUBDIRS = g3d \
          g3dexport \
          g3dimport \
          g3dmouse \
          g3dcom \
          g3dui \
          g3durmanager \
          r3d
#          r3dnet

if HAVE_GTK3
    bin_PROGRAMS = gloss3d-gtk3
    gloss3d_gtk3_SOURCES = $(BINGLOSS3D_C_FILES)

    gloss3d_gtk3_LDADD = g3d/libgloss-g3d.la \
                         g3dexport/libgloss-g3dexport.la \
                         g3dimport/libgloss-g3dimport.la \
                         g3dmouse/libgloss-g3dmouse.la \
                         g3dcom/libgloss-g3dcom.la \
                         g3dui/libgloss-g3dui-gtk3.la \
                         g3durmanager/libgloss-g3durmanager.la \
                         r3d/libgloss-r3d.la
#                         r3dnet/libgloss-r3dnet.la

    BINGLOSS3D_C_FILES   = main.c \
                           list.c
endif

if HAVE_MOTIF
    bin_PROGRAMS = gloss3d-motif
    gloss3d_motif_SOURCES = $(BINGLOSS3D_C_FILES)

    gloss3d_motif_LDADD = src/g3d/libgloss-g3d.la \
                          src/g3dexport/libgloss-g3dexport.la \
                          src/g3dimport/libgloss-g3dimport.la \
                          src/g3dmouse/libgloss-g3dmouse.la \
                          src/g3dcom/libgloss-g3dcom.la \
                          src/g3dui/libgloss-g3dui-motif.la \
                          src/g3durmanager/libgloss-g3durmanager.la \
                          src/r3d/libgloss-r3d.la
#                          src/r3dnet/libgloss-r3dnet.la

    BINGLOSS3D_C_FILES   = main.c \
                           GLwDrawA.c \
                           GLwMDrawA.c \
                           list.c
endif

if HAVE_WIN32
    bin_PROGRAMS = gloss3d-win32
    gloss3d_win32_SOURCES = $(BINGLOSS3D_C_FILES)

    gloss3d_win32_LDADD = g3d/libgloss-g3d.la \
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
                       include/g3d.h \
                       include/g3dimport.h \
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
                       xpm/addvertex.xpm \
                       xpm/bridge.xpm \
                       xpm/cube.xpm \
                       xpm/cylinder.xpm \
                       xpm/delete.xpm \
                       xpm/edgemode.xpm \
                       xpm/extrude.xpm \
                       xpm/facemode.xpm \
                       xpm/horz.xpm \
                       xpm/knife.xpm \
                       xpm/makeedit.xpm \
                       xpm/matedit.xpm \
                       xpm/move.xpm \
                       xpm/newfile.xpm \
                       xpm/nextframe.xpm \
                       xpm/objectmode.xpm \
                       xpm/openfile.xpm \
                       xpm/pick.xpm \
                       xpm/playanim.xpm \
                       xpm/prevframe.xpm \
                       xpm/record.xpm \
                       xpm/redo.xpm \
                       xpm/render.xpm \
                       xpm/renderw.xpm \
                       xpm/rotate.xpm \
                       xpm/save.xpm \
                       xpm/plnico.xpm \
                       xpm/mapico.xpm \
                       xpm/scale.xpm \
                       xpm/shor.xpm \
                       xpm/sobj.xpm \
                       xpm/sphere.xpm \
                       xpm/stopanim.xpm \
                       xpm/stoprender.xpm \
                       xpm/undo.xpm \
                       xpm/unzoomtime.xpm \
                       xpm/vertexmode.xpm \
                       xpm/vert.xpm \
                       xpm/xaxis.xpm \
                       xpm/yaxis.xpm \
                       xpm/zaxis.xpm \
                       xpm/zoomtime.xpm \
                       xpm/skinmode.xpm \
                       xpm/uvwmapmode.xpm \
                       xpm/ligico.xpm \
                       xpm/bonico.xpm \
                       xpm/mesico.xpm \
                       xpm/defico.xpm \
                       xpm/symico.xpm \
                       xpm/cube.xpm \
                       xpm/cubico.xpm \
                       xpm/torico.xpm \
                       xpm/cylico.xpm \
                       xpm/sphico.xpm \
                       xpm/camico.xpm \
                       xpm/saveas.xpm \
                       xpm/scnico.xpm \
                       xpm/ffdico.xpm \
                       xpm/eyeopn.xpm \
                       xpm/eyesht.xpm \
                       xpm/disico.xpm \
                       xpm/enaico.xpm \
                       xpm/translate_view.xpm \
                       xpm/rotate_view.xpm \
                       xpm/maximize_view.xpm \
                       xpm/zoom_view.xpm \
                       Makefile.win32

backup:
	tar -cvzf ../Motif-`date +%Y%m%d`.tar.gz * && \
        ./backup.sh ../Motif-`date +%Y%m%d`.tar.gz && \
        rm -f ../Motif-`date +%Y%m%d`.tar.gz
.PHONY: backup

count:
	cloc * --exclude-lang=m4,make,Bourne\ Shell,Bourne\ Again\ Shell
.PHONY: count

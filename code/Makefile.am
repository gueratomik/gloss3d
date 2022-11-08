ACLOCAL_AMFLAGS= -I m4

if HAVE_WIN32
    AM_CFLAGS= -mwindows
endif

SUBDIRS = g3dengine \
          g3dexport \
          g3dimport \
          g3dmouse \
          g3dcom \
          g3dui \
          g3durmanager \
          makeup3d \
          qiss3d
#          r3dnet


bin_PROGRAMS = gloss3d-gtk3
gloss3d_gtk3_SOURCES = $(BINGLOSS3D_C_FILES)

gloss3d_gtk3_LDADD = g3dengine/libgloss-g3dengine.la \
                     g3dexport/libgloss-g3dexport.la \
                     g3dimport/libgloss-g3dimport.la \
                     g3dmouse/libgloss-g3dmouse.la \
                     g3dcom/libgloss-g3dcom.la \
                     g3dui/libgloss-g3dui-gtk3.la \
                     g3durmanager/libgloss-g3durmanager.la \
                     makeup3d/libgloss-makeup3d.la \
                     qiss3d/libgloss-q3d.la \
                     list/libgloss-list.la
#                     r3dnet/libgloss-r3dnet.la

BINGLOSS3D_C_FILES   = main.c

EXTRA_DIST   = include/g3dexport.h \
                       include/g3dexportv2.h \
                       include/g3dexportv3.h \
                       include/g3dengine/g2dvector.h \
                       include/g3dengine/g3dcolor.h \
                       include/g3dengine/g3dcurve.h \
                       include/g3dengine/g3dengine.h \
                       include/g3dengine/g3dmorpher.h \
                       include/g3dengine/g3dimage.h \
                       include/g3dengine/g3dinstance.h \
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
                       include/g3dengine/g3dtag.h \
                       include/g3dengine/g3dtext.h \
                       include/g3dengine/g3dtinyvector.h \
                       include/g3dengine/g3dparticleemitter.h \
                       include/g3dengine/g3dskin.h \
                       include/g3dengine/g3dvector.h \
                       include/makeup3d/makeup3d.h \
                       include/g3d.h \
                       include/g3dimport.h \
                       include/g3dimportv2.h \
                       include/g3dimportv3.h \
                       include/g3dmouse.h \
                       include/g3dui.h \
                       include/g3dui_motif.h \
                       include/g3dui_gtk3.h \
                       include/g3dcom.h \
                       include/g3durmanager.h \
                       include/list.h \
                       include/r3d.h \
                       include/qiss3d/q3d.h \
                       include/r3dnet.h \
                       include/TabbedBook.h \
                       include/ColorEdit.h \
                       include/MaterialEdit.h \
                       include/GLwDrawA.h \
                       include/GLwDrawAP.h \
                       include/GLwMDrawA.h \
                       include/GLwMDrawAP.h \
                       include/signatures \
                       include/signaturesv2.h \
                       include/signaturesv3.h \
                       include/siggen.sh \
                       include/xpm/addvertex.xpm \
                       include/xpm/axismode.xpm \
                       include/xpm/bonico.xpm \
                       include/xpm/bridge.xpm \
                       include/xpm/bucket.xpm \
                       include/xpm/camico.xpm \
                       include/xpm/cube.xpm \
                       include/xpm/cubico.xpm \
                       include/xpm/cylico.xpm \
                       include/xpm/cylinder.xpm \
                       include/xpm/defico.xpm \
                       include/xpm/delete.xpm \
                       include/xpm/disico.xpm \
                       include/xpm/edgemode.xpm \
                       include/xpm/enaico.xpm \
                       include/xpm/eraser.xpm \
                       include/xpm/extrude.xpm \
                       include/xpm/eyeopn.xpm \
                       include/xpm/eyesht.xpm \
                       include/xpm/facemode.xpm \
                       include/xpm/ffdico.xpm \
                       include/xpm/horz.xpm \
                       include/xpm/knife.xpm \
                       include/xpm/ligico.xpm \
                       include/xpm/makeedit.xpm \
                       include/xpm/mapico.xpm \
                       include/xpm/matedit.xpm \
                       include/xpm/maximize_view.xpm \
                       include/xpm/mesico.xpm \
                       include/xpm/move.xpm \
                       include/xpm/newfile.xpm \
                       include/xpm/nextframe.xpm \
                       include/xpm/objectmode.xpm \
                       include/xpm/pathmode.xpm \
                       include/xpm/openfile.xpm \
                       include/xpm/pen.xpm \
                       include/xpm/pick.xpm \
                       include/xpm/playanim.xpm \
                       include/xpm/plnico.xpm \
                       include/xpm/prevframe.xpm \
                       include/xpm/record.xpm \
                       include/xpm/redo.xpm \
                       include/xpm/renderw.xpm \
                       include/xpm/render.xpm \
                       include/xpm/rotate_view.xpm \
                       include/xpm/rotate.xpm \
                       include/xpm/saveas.xpm \
                       include/xpm/saveimageas.xpm \
                       include/xpm/save.xpm \
                       include/xpm/saveimage.xpm \
                       include/xpm/scale.xpm \
                       include/xpm/scnico.xpm \
                       include/xpm/sknico.xpm \
                       include/xpm/insico.xpm \
                       include/xpm/sculptmode.xpm \
                       include/xpm/selectarea.xpm \
                       include/xpm/selectrandom.xpm \
                       include/xpm/shor.xpm \
                       include/xpm/skinmode.xpm \
                       include/xpm/sobj.xpm \
                       include/xpm/sphere.xpm \
                       include/xpm/sphico.xpm \
                       include/xpm/splico.xpm \
                       include/xpm/srvico.xpm \
                       include/xpm/stopanim.xpm \
                       include/xpm/stoprender.xpm \
                       include/xpm/subico.xpm \
                       include/xpm/symico.xpm \
                       include/xpm/torico.xpm \
                       include/xpm/tracker.xpm \
                       include/xpm/translate_view.xpm \
                       include/xpm/tubico.xpm \
                       include/xpm/txtico.xpm \
                       include/xpm/undo.xpm \
                       include/xpm/morpher.xpm \
                       include/xpm/unselectarea.xpm \
                       include/xpm/unzoomtime.xpm \
                       include/xpm/uvwmapmode.xpm \
                       include/xpm/vertexmode.xpm \
                       include/xpm/vert.xpm \
                       include/xpm/vibrate.xpm \
                       include/xpm/wirico.xpm \
                       include/xpm/xaxis.xpm \
                       include/xpm/yaxis.xpm \
                       include/xpm/zaxis.xpm \
                       include/xpm/zoomtime.xpm \
                       include/xpm/zoom_view.xpm \
                       include/xpm/scale_keys.xpm \
                       include/xpm/pick_keys.xpm \
                       include/xpm/vertexmode_test.xpm \
                       include/xpm/objectmode_test.xpm \
                       include/xpm/edgemode_test.xpm \
                       include/xpm/facemode_test.xpm \
                       include/xpm/skinmode_test.xpm \
                       include/xpm/uvwmapmode_test.xpm \
                       include/xpm/axismode_test.xpm \
                       include/xpm/pemico.xpm \
                       include/xpm/brush_0.c \
                       include/xpm/brush_1.c \
                       include/xpm/brush_2.c \
                       include/xpm/brush_3.c \
                       include/xpm/brush_4.c \
                       include/xpm/brush_5.c


backup:
	tar -cvzf ../Motif-`date +%Y%m%d`.tar.gz `find . -type f | grep -v "\.o"` && \
        ./backup.sh ../Motif-`date +%Y%m%d`.tar.gz && \
        rm -f ../Motif-`date +%Y%m%d`.tar.gz
.PHONY: backup

count:
	cloc main.c list.c include g3dui g3dengine g3dmouse qiss3d makeup3d g3durmanager g3dexport g3dimport g3dcom --exclude-lang=CSS,Javascript,HTML,m4,make,Bourne\ Shell,Bourne\ Again\ Shell
.PHONY: count

ACLOCAL_AMFLAGS= -I m4

if HAVE_WIN32
    AM_LDFLAGS = -shared -no-undefined `pkg-config --libs gtk+-3.0` \
		                       `pkg-config --libs xkbcommon` \
                                       -lfontconfig \
                                       -L../g3dmouse/.libs/ -lgloss-g3dmouse \
                                       -L../g3durmanager/.libs/ -lgloss-g3durmanager \
                                       -L../makeup3d/.libs/ -lgloss-makeup3d \
                                       -L../qiss3d/.libs/ -lgloss-q3d \
                                       -L../g3dimport/.libs/ -lgloss-g3dimport \
                                       -L../g3dexport/.libs/ -lgloss-g3dexport \
                                       -L../g3dengine/.libs/ -lgloss-g3dengine \
                                       -L../list/.libs/ -lgloss-list \
                                       -lavifil32 -lmsvfw32 -lshlwapi -lgdi32
endif

lib_LTLIBRARIES = libgloss-g3dui-gtk3.la
libgloss_g3dui_gtk3_la_SOURCES = $(LIBGLOSSG3DUI_C_FILES)

LIBGLOSSG3DUI_C_FILES = common/g3dui.c \
                        common/g3duiboard.c \
                        common/g3duiboneedit.c \
                        common/g3duibonelinker.c \
                        common/g3duicameraedit.c \
                        common/g3duiclipboard.c \
                        common/g3duiconf.c \
                        common/g3duicoordinatesedit.c \
                        common/g3duicubeedit.c \
                        common/g3duicurrentedit.c \
                        common/g3duicylinderedit.c \
                        common/g3duiexport.c \
                        common/g3duiffdedit.c \
                        common/g3duiinstanceedit.c \
                        common/g3duikeyedit.c \
                        common/g3duilightedit.c \
                        common/g3duimain.c \
                        common/g3duimaterialboard.c \
                        common/g3duimaterialedit.c \
                        common/g3duimateriallist.c \
                        common/g3duimaterialnameedit.c \
                        common/g3duimenu.c \
                        common/g3duimeshedit.c \
                        common/g3duimeshposelist.c \
                        common/g3duimodebar.c \
                        common/g3duimousetool.c \
                        common/g3duiobjectboard.c \
                        common/g3duiobjectlist.c \
                        common/g3duiobjectedit.c \
                        common/g3duiparticleemitteredit.c \
                        common/m3duipatternlist.c \
                        common/g3duiplaneedit.c \
                        common/g3duiquad.c \
                        common/g3duirenderbuffer.c \
                        common/g3duirenderedit.c \
                        common/g3duirenderprocess.c \
                        common/g3duirenderwindow.c \
                        common/g3duisphereedit.c \
                        common/g3duisplinerevolveredit.c \
                        common/g3duisubdivideredit.c \
                        common/g3duisymmetryedit.c \
                        common/g3duitextedit.c \
                        common/g3duitextureedit.c \
                        common/g3duitimeboard.c \
                        common/g3duitimeline.c \
                        common/g3duitoolbar.c \
                        common/g3duitorusedit.c \
                        common/g3duitrackertagedit.c \
                        common/g3duitubeedit.c \
                        common/g3duiuvmapedit.c \
                        common/g3duiview.c \
                        common/g3duiviewexport.c \
                        common/g3duiweightgrouplist.c \
                        common/g3duiwireframeedit.c \
                        common/mousetool/m3duibuckettooledit.c \
                        common/mousetool/g3duicutmeshtooledit.c \
                        common/mousetool/m3duipentooledit.c \
                        common/mousetool/g3duipicktooledit.c \
                        common/mousetool/g3duimovetooledit.c \
                        common/mousetool/g3duisculpttooledit.c \
                        common/m3dui.c \
                        common/m3duiboard.c \
                        common/m3duicolorpicker.c \
                        common/m3duimain.c \
                        common/m3duimodebar.c \
                        common/m3duitoolbar.c \
                        common/m3duiview.c \
                        gtk3/g3dui.c \
                        gtk3/g3duiboard.c \
                        gtk3/g3duiboneedit.c \
                        gtk3/g3duicameraedit.c \
                        gtk3/g3duicom.c \
                        gtk3/g3duicoordinatesedit.c \
                        gtk3/g3duicubeedit.c \
                        gtk3/g3duicylinderedit.c \
                        gtk3/g3duiffdedit.c \
                        gtk3/g3duiinstanceedit.c \
                        gtk3/g3duikeyedit.c \
                        gtk3/g3duilightedit.c \
                        gtk3/g3duimain.c \
                        gtk3/g3duimateriallist.c \
                        gtk3/g3duimaterialedit.c \
                        gtk3/g3duimaterialboard.c \
                        gtk3/g3duimenubar.c \
                        gtk3/g3duimeshedit.c \
                        gtk3/g3duimodebar.c \
                        gtk3/g3duimorpheredit.c \
                        gtk3/g3duimorpherkeyedit.c \
                        gtk3/g3duiobjectboard.c \
                        gtk3/g3duiobjectedit.c \
                        gtk3/g3duiobjectlist.c \
                        gtk3/g3duiplaneedit.c \
                        gtk3/g3duiparticleemitteredit.c \
                        gtk3/g3duiproceduralbrickedit.c \
                        gtk3/g3duiproceduralchessedit.c \
                        gtk3/g3duiproceduralgradientedit.c \
                        gtk3/g3duiproceduralnoiseedit.c \
                        gtk3/g3duiquad.c \
                        gtk3/g3duirenderedit.c \
                        gtk3/g3duirenderwindow.c \
                        gtk3/g3duisphereedit.c \
                        gtk3/g3duisplinerevolveredit.c \
                        gtk3/g3duisubdivideredit.c \
                        gtk3/g3duisymmetryedit.c \
                        gtk3/g3duitextedit.c \
                        gtk3/g3duitextureedit.c \
                        gtk3/g3duitimeboard.c \
                        gtk3/g3duitimeline.c \
                        gtk3/g3duitoolbar.c \
                        gtk3/g3duitorusedit.c \
                        gtk3/g3duitrackertagedit.c \
                        gtk3/g3duitubeedit.c \
                        gtk3/g3duiuvmapedit.c \
                        gtk3/g3duiview.c \
                        gtk3/g3duiwireframeedit.c \
                        gtk3/q3dfilters.c \
                        gtk3/q3dpreview.c \
                        gtk3/menu/condition.c \
                        gtk3/menu/mainmenu.c \
                        gtk3/menu/materialboardmenu.c \
                        gtk3/menu/objectboardmenu.c \
                        gtk3/menu/renderwindowmenu.c \
                        gtk3/menu/timelinemenu.c \
                        gtk3/menu/uveditormenu.c \
                        gtk3/menu/uvviewmenu.c \
                        gtk3/menu/viewmenu.c \
                        gtk3/mousetool/m3duimousetooledit.c \
                        gtk3/mousetool/m3duibuckettooledit.c \
                        gtk3/mousetool/m3duipentooledit.c \
                        gtk3/mousetool/g3duimousetooledit.c \
                        gtk3/mousetool/g3duicutmeshtooledit.c \
                        gtk3/mousetool/g3duipicktooledit.c \
                        gtk3/mousetool/g3duimovetooledit.c \
                        gtk3/mousetool/g3duisculpttooledit.c \
                        gtk3/m3dui.c \
                        gtk3/m3duiboard.c \
                        gtk3/m3duichannelimagecreator.c \
                        gtk3/m3duicolorpicker.c \
                        gtk3/m3duimain.c \
                        gtk3/m3duimodebar.c \
                        gtk3/m3duipatternlist.c \
                        gtk3/m3duitoolbar.c \
                        gtk3/m3duiview.c \
                        gtk3/css/default.c


#if HAVE_MOTIF
#    lib_LTLIBRARIES = libgloss-g3dui-motif.la
#    libgloss_g3dui_motif_la_SOURCES = $(LIBGLOSSG3DUI_C_FILES)

#    LIBGLOSSG3DUI_C_FILES = common/g3dui.c \
#                            common/g3duiboneedit.c \
#                            common/g3duibonelinker.c \
#                            common/g3duiclipboard.c \
#                            common/g3duiconf.c \
#                            common/g3duicoordinatesedit.c \
#                            common/g3duicubeedit.c \
#                            common/g3duicurrentedit.c \
#                            common/g3duicylinderedit.c \
#                            common/g3duiffdedit.c \
#                            common/g3duikeyedit.c \
#                            common/g3duilightedit.c \
#                            common/g3duilist.c \
#                            common/g3duimaterialboard.c \
#                            common/g3duimaterialedit.c \
#                            common/g3duimateriallist.c \
#                            common/g3duimenubar.c \
#                            common/g3duimeshedit.c \
#                            common/g3duimodebar.c \
#                            common/g3duiobjectedit.c \
#                            common/g3duiplaneedit.c \
#                            common/g3duiquad.c \
#                            common/g3duirenderedit.c \
#                            common/g3duirenderprocess.c \
#                            common/g3duirenderwindow.c \
#                            common/g3duisphereedit.c \
#                            common/g3duisymmetryedit.c \
#                            common/g3duitextureedit.c \
#                            common/g3duitimeboard.c \
#                            common/g3duitimeline.c \
#                            common/g3duitoolbar.c \
#                            common/g3duitorusedit.c \
#                            common/g3duiuvmapedit.c \
#                            common/g3duiview.c \
#                            common/g3duiweightgrouplist.c \
#                            common/mousetool/g3duicurrentmousetool.c \
#                            common/mousetool/g3duicutmeshtooledit.c \
#                            common/mousetool/g3duipicktooledit.c \
#                            common/mousetool/g3duisculpttooledit.c \
#                            motif/TabbedBook.c \
#                            motif/ColorEdit.c \
#                            motif/SpinButton.c \
#                            motif/g3dui.c \
#                            motif/g3duicom.c \
#                            motif/g3duicoordinatesedit.c \
#                            motif/g3duicubeedit.c \
#                            motif/g3duicurrentedit.c \
#                            motif/g3duicylinderedit.c \
#                            motif/g3duiffdedit.c \
#                            motif/g3duikeyedit.c \
#                            motif/g3duilightedit.c \
#                            motif/g3duilist.c \
#                            motif/g3duimainboard.c \
#                            motif/g3duimaterialboard.c \
#                            motif/g3duimaterialedit.c \
#                            motif/g3duimateriallist.c \
#                            motif/g3duimeshedit.c \
#                            motif/g3duimenubar.c \
#                            motif/g3duimodebar.c \
#                            motif/g3duiplaneedit.c \
#                            motif/g3duiobjectboard.c \
#                            motif/g3duiobjectedit.c \
#                            motif/g3duiquad.c \
#                            motif/g3duirenderedit.c \
#                            motif/g3duirenderwindow.c \
#                            motif/g3duisphereedit.c \
#                            motif/g3duisymmetryedit.c \
#                            motif/g3duitimeline.c \
#                            motif/g3duitimeboard.c \
#                            motif/g3duitoolbar.c \
#                            motif/g3duitorusedit.c \
#                            motif/g3duiuvmapedit.c \
#                            motif/g3duiview.c \
#                            motif/g3duiweightgrouplist.c \
#                            motif/mousetool/g3duicurrentmousetool.c \
#                            motif/mousetool/g3duicutmeshtooledit.c \
#                            motif/mousetool/g3duipicktooledit.c \
#                            motif/mousetool/g3duisculpttooledit.c
#endif

#                            motif/g3duiboneedit.c \
#                            motif/g3duitextureedit.c

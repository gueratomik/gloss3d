ACLOCAL_AMFLAGS= -I m4

AM_CFLAGS = -I$(top_srcdir)/include \
            -I$(top_srcdir) -Wimplicit-function-declaration -Wformat

if HAVE_GTK3
    AM_LDFLAGS = -Wl,--no-as-needed `pkg-config --libs gtk+-3.0`
    lib_LTLIBRARIES = libgloss-g3dui-gtk3.la
    libgloss_g3dui_gtk3_la_SOURCES = $(LIBGLOSSG3DUI_C_FILES)

    LIBGLOSSG3DUI_C_FILES = common/g3dui.c \
                            common/g3duiexport.c \
                            common/g3duiboneedit.c \
                            common/g3duibonelinker.c \
                            common/g3duicameraedit.c \
                            common/g3duiclipboard.c \
                            common/g3duiconf.c \
                            common/g3duicoordinatesedit.c \
                            common/g3duicubeedit.c \
                            common/g3duicurrentedit.c \
                            common/g3duicylinderedit.c \
                            common/g3duiffdedit.c \
                            common/g3duikeyedit.c \
                            common/g3duilightedit.c \
                            common/g3duilist.c \
                            common/g3duimaterialboard.c \
                            common/g3duimaterialedit.c \
                            common/g3duimateriallist.c \
                            common/g3duimenubar.c \
                            common/g3duimeshedit.c \
                            common/g3duimeshposelist.c \
                            common/g3duimodebar.c \
                            common/g3duiobjectedit.c \
                            common/g3duiplaneedit.c \
                            common/g3duiquad.c \
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
                            common/g3duiuvmapedit.c \
                            common/g3duiuvmapeditor.c \
                            common/g3duiview.c \
                            common/g3duiviewexport.c \
                            common/g3duiweightgrouplist.c \
                            common/g3duiwireframeedit.c \
                            common/mousetool/g3duicurrentmousetool.c \
                            common/mousetool/g3duicutmeshtooledit.c \
                            common/mousetool/g3duipicktooledit.c \
                            common/mousetool/g3duisculpttooledit.c \
                            gtk3/g3dui.c \
                            gtk3/g3duiboneedit.c \
                            gtk3/g3duibonelinker.c \
                            gtk3/g3duicameraedit.c \
                            gtk3/g3duicom.c \
                            gtk3/g3duicoordinatesedit.c \
                            gtk3/g3duicubeedit.c \
                            gtk3/g3duicurrentedit.c \
                            gtk3/g3duicylinderedit.c \
                            gtk3/g3duiffdedit.c \
                            gtk3/g3duikeyedit.c \
                            gtk3/g3duilightedit.c \
                            gtk3/g3duilist.c \
                            gtk3/g3duimainboard.c \
                            gtk3/g3duimaterialboard.c \
                            gtk3/g3duimaterialedit.c \
                            gtk3/g3duimateriallist.c \
                            gtk3/g3duimenubar.c \
                            gtk3/g3duimeshedit.c \
                            gtk3/g3duimeshposelist.c \
                            gtk3/g3duimodebar.c \
                            gtk3/g3duiobjectboard.c \
                            gtk3/g3duiobjectedit.c \
                            gtk3/g3duiplaneedit.c \
                            gtk3/g3duiquad.c \
                            gtk3/g3duirenderedit.c \
                            gtk3/g3duirenderprocess.c \
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
                            gtk3/g3duiuvmapedit.c \
                            gtk3/g3duiuvmapeditor.c \
                            gtk3/g3duiuvmapeditortoolbar.c \
                            gtk3/g3duiview.c \
                            gtk3/g3duiweightgrouplist.c \
                            gtk3/g3duiwireframeedit.c \
                            gtk3/r3dpreview.c \
                            gtk3/mousetool/g3duicurrentmousetool.c \
                            gtk3/mousetool/g3duicutmeshtooledit.c \
                            gtk3/mousetool/g3duipicktooledit.c \
                            gtk3/mousetool/g3duisculpttooledit.c
endif

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

AC_INIT([gloss3d],[0.99.1])
AC_CANONICAL_TARGET
AC_CONFIG_HEADERS([include/config.h])
LT_INIT
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([subdir-objects])
#AM_INIT_AUTOMAKE(gloss3d,0.99.1)
AM_SILENT_RULES([yes])

AC_PROG_CC([gcc])
AC_PROG_MAKE_SET

AC_LANG(C)

# Checks for header files.
m4_warn([obsolete],
[The preprocessor macro `STDC_HEADERS' is obsolete.
  Except in unusual embedded environments, you can safely include all
  ISO C90 headers unconditionally.])dnl
# Autoupdate added the next two lines to ensure that your configure
# script's behavior did not change.  They are probably safe to remove.

AM_CONDITIONAL(HAVE_MOTIF,false)
AM_CONDITIONAL(HAVE_GTK3,false)
AM_CONDITIONAL(HAVE_WIN32,false)
AM_CONDITIONAL(HAVE_EXPAT,false)

#Check for MingW32 (default)
AC_CHECK_PROG(have_win32, x86_64-w64-mingw32-cc, yes, no)

if test x"$have_win32" = xyes
then
    AC_DEFINE(HAVE_WIN32,1,[Build for Windows])
    AM_CONDITIONAL(HAVE_WIN32,true)

    CFLAGS="$CFLAGS -I/mingw64/include -I/mingw64/include/freetype2"
fi

AC_CHECK_HEADERS(ft2build.h, have_freetype2=yes, have_freetype2=no)

if test x"$have_freetype2" = xyes
then
    CFLAGS="$CFLAGS `pkg-config --cflags freetype2`"
    LDFLAGS="$LDFLAGS `pkg-config --libs freetype2`"
fi

# Checks for expat.
AC_CHECK_HEADERS(expat.h, have_expat=yes, have_expat=no)
AC_CHECK_LIB(expat, XML_SetElementHandler, have_expat=yes, have_expat=no)

if test x"$have_expat" = xyes
then
    AC_DEFINE(HAVE_EXPAT,1,[Use expat])
    
    LDFLAGS="$LDFLAGS -lexpat"
fi

# Checks for libjpeg.
AC_CHECK_HEADERS(jpeglib.h, have_jpeg=yes, have_jpeg=no)
AC_CHECK_LIB(jpeg, jpeg_destroy_decompress, have_jpeg=yes, have_jpeg=no)

# Checks for libpng.
AC_CHECK_HEADERS(png.h, have_png=yes, have_jpeg=no)
AC_CHECK_LIB(png, png_read_info, have_png=yes, have_png=no)

#Check for GTK3
PKG_CHECK_MODULES(MT_IM_GTK3, gtk+-3.0 >= 3.0,[have_gtk3="yes"],[have_gtk3="no"])

if test x"$have_gtk3" = xyes
then
    AC_DEFINE(HAVE_GTK3,1,[Use GTK+3])

#    LDFLAGS="$LDFLAGS `pkg-config --libs gtk+-3.0` `pkg-config --libs gdk-3.0` `pkg-config --libs pango`"
    CFLAGS="$CFLAGS `pkg-config --cflags gtk+-3.0` `pkg-config --cflags gdk-3.0` `pkg-config --cflags pango`"
fi

if test x"$have_win32" = xyes
then
#    LDFLAGS="-Wl,--no-as-needed -lXpm -lglew32 -ljpeg -lpng -lm $LDFLAGS -ljpeg -lpng -lm -lpthread -lc"
    CFLAGS="-g -I$(top_srcdir)/include $CFLAGS -Wimplicit-function-declaration -Wformat"
else
#    LDFLAGS="-Wl,--no-as-needed -lXpm -lGL -lGLU -lX11 $LDFLAGS -ljpeg -lpng -lm -lpthread -lc"
    CFLAGS="-g -I$(top_srcdir)/include -L/usr/lib -L/usr/lib/i386-linux-gnu -L/usr/lib/x86_64-linux-gnu $CFLAGS -Wimplicit-function-declaration -Wformat"
fi

AC_CONFIG_FILES([g3dengine/Makefile    \
                 g3dexport/Makefile    \
                 g3dimport/Makefile    \
                 g3dmouse/Makefile     \
                 g3dcom/Makefile       \
                 g3dui/Makefile        \
                 g3durmanager/Makefile \
                 makeup3d/Makefile     \
                 qiss3d/Makefile       \
                 list/Makefile         \
                 Makefile])
AC_OUTPUT

#           r3dnet/Makefile

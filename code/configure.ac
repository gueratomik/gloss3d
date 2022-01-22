AC_INIT(gloss3d,0.5.4)
AC_CANONICAL_SYSTEM
AC_CONFIG_FILES([Makefile])
AC_CONFIG_HEADERS([include/config.h])
AC_PROG_LIBTOOL
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([subdir-objects])
AM_INIT_AUTOMAKE(gloss3d,0.5.4)
AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PROG_MAKE_SET

AC_LANG(C)

# Checks for header files.
AC_HEADER_STDC

#Check for MingW32 (default)
#AC_CHECK_PROG(have_win32, i386-mingw32-gcc, yes, no)

AM_CONDITIONAL(HAVE_MOTIF,false)
AM_CONDITIONAL(HAVE_GTK3,false)
AM_CONDITIONAL(HAVE_WIN32,false)
AM_CONDITIONAL(HAVE_EXPAT,false)

if test x"$have_win32" = xyes
then
    AC_ARG_WITH(win32,AC_HELP_STRING([--with-win32], [build for Windows(gtk3)]))

    if test x"$with_win32" = xyes
    then
        AC_DEFINE(WITH_WIN32,1,[Build for Windows])

        AM_CONDITIONAL(HAVE_MOTIF,false)
        AM_CONDITIONAL(HAVE_GTK3,false)
        AC_DEFINE(HAVE_EXPAT_H,1,[Use expat])
        AM_CONDITIONAL(HAVE_WIN32,true)
    fi
else
    # Checks for expat.
    AC_CHECK_HEADERS(expat.h, have_expat=yes, have_expat=no)
    AC_CHECK_LIB(expat, XML_SetElementHandler, have_expat=yes, have_expat=no)

    # Checks for libjpeg.
    AC_CHECK_HEADERS(jpeglib.h, have_jpeg=yes, have_jpeg=no)
    AC_CHECK_LIB(jpeg, jpeg_destroy_decompress, have_jpeg=yes, have_jpeg=no)

    # Checks for libpng.
    AC_CHECK_HEADERS(png.h, have_png=yes, have_jpeg=no)
    AC_CHECK_LIB(png, png_read_info, have_png=yes, have_png=no)

    #Check for Motif (default)
    AC_CHECK_HEADERS([Xm/Xm.h], have_motif=yes, have_motif=no)

    if test x"$have_motif" = xyes
    then
        AC_ARG_WITH(motif,AC_HELP_STRING([--with-motif], [build Motif Front-End (default)]))

        if test x"$with_motif" = xyes
        then
            AC_DEFINE(WITH_MOTIF,1,[Use Motif])

            AM_CONDITIONAL(HAVE_MOTIF,true)
            AM_CONDITIONAL(HAVE_GTK3,false)
            AM_CONDITIONAL(HAVE_WIN32,false)
        fi
    fi

    #Check for GTK3
    PKG_CHECK_MODULES(MT_IM_GTK3, gtk+-3.0 >= 3.0,[have_gtk3="yes"],[have_gtk3="no"])
    AC_ARG_WITH(gtk3,AC_HELP_STRING([--with-gtk3], [build GTK+3 Front-End]))

    if test x"$have_gtk3" = xyes
    then
        if test x"$with_gtk3" = xyes
        then
            AC_DEFINE(WITH_GTK3,1,[Use GTK+3])

            AM_CONDITIONAL(HAVE_MOTIF,false)
            AM_CONDITIONAL(HAVE_GTK3,true)
            AM_CONDITIONAL(HAVE_WIN32,false)

            CFLAGS="$CFLAGS `pkg-config --cflags gtk+-3.0`"
            LDFLAGS="$LDFLAGS `pkg-config --libs gtk+-3.0`"
        fi
    fi
fi

if test x"$with_gtk3" != xyes
then
    if test x"$with_motif" != xyes
    then
        if test x"$with_win32" != xyes
        then
            AC_MSG_ERROR([No front-end chosen. Please use --with-gtk3 or --with-motif.])
        fi
    fi
fi

AC_OUTPUT([g3dengine/Makefile          \
           g3dexport/Makefile    \
           g3dimport/Makefile    \
           g3dmouse/Makefile     \
           g3dcom/Makefile     \
           g3dui/Makefile        \
           g3durmanager/Makefile \
           makeup3d/Makefile \
           qiss3d/Makefile])

#           r3dnet/Makefile

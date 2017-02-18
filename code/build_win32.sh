./configure --with-win32 CC=i386-mingw32-gcc CFLAGS="-I/root/mingw32/gtk3/include/gtk-3.0 \
                                                     -I/root/mingw32/gtk3/include/pango-1.0 \
                                                     -I/root/mingw32/gtk3/include \
                                                     -I/root/mingw32/gtk3/include/cairo \
                                                     -I/root/mingw32/gtk3/include/atk-1.0 \
                                                     -I/root/mingw32/gtk3/include/gdk-pixbuf-2.0 \
                                                     -I/root/mingw32/gtk3/lib/glib-2.0/include \
                                                     -I/root/mingw32/gtk3/include/glib-2.0 \
                                                     -I/root/mingw32/gtk3/include/libpng15 \
                                                     -I/root/mingw32/expat/include" --build i686-pc-linux-gnu  --host i586-mingw32msvc

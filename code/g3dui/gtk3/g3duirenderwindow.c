/******************************************************************************/
/*  GLOSS3D is free software: you can redistribute it and/or modify           */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  GLOSS3D is distributed in the hope that it will be useful,                */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with GLOSS3D.  If not, see http://www.gnu.org/licenses/.            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2020         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#include <config.h>
#include <g3dui_gtk3.h>

/******************************************************************************/
static GTK3G3DUIRENDERWINDOW *gtk3_g3duirenderwindow_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t size = sizeof ( GTK3G3DUIRENDERWINDOW );
    GTK3G3DUIRENDERWINDOW *gtk3rwin = ( GTK3G3DUIRENDERWINDOW * ) calloc ( 0x01, size );

    if ( gtk3rwin == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    gtk3rwin->core.gui = &gtk3gui->core;


    return gtk3rwin;
}

/******************************************************************************/
void Draw ( GtkWidget *widget, cairo_t *cr, gpointer user_data ) {
    GTK3G3DUIRENDERWINDOW *gtk3rwin = ( GTK3G3DUIRENDERWINDOW * ) user_data;
    GdkDisplay *gdkdpy   = gtk_widget_get_display ( widget );
    GdkWindow  *gdkwin   = gtk_widget_get_window  ( widget );
    G3DUI *gui = ( G3DUI * ) gtk3rwin->core.gui;
    uint32_t width  = gtk_widget_get_allocated_width  ( widget ),
             height = gtk_widget_get_allocated_height ( widget );
    uint32_t x = 0, y = 0;

    /* 
     * fill the background. We don't let the double buffering doing it
     * because it causes some flickering, I don't know why.
     */
    cairo_rectangle ( cr, 0, 0, width, height  );
    cairo_fill ( cr );
    /*cairo_paint ( cr );*/

    #ifdef __linux__
    Display    *dis      = gdk_x11_display_get_xdisplay ( gdkdpy );
    Window      win      = gdk_x11_window_get_xid ( gdkwin );
    #endif
    #ifdef __MINGW32__
    HDC dc = GetDC ( gtk3rwin->core.rbuf.hWnd );
    #endif

    /*** Tell cairo to shut the F*** up ***/
    GdkRectangle arec;

    arec.x = arec.y = 0;
    arec.width = arec.height = 1;

    gdk_window_invalidate_rect ( gtk_widget_get_window ( widget ), &arec, FALSE );
    /*************************************/

    #ifdef __linux__
    if ( width  > gtk3rwin->core.rbuf.ximg->width  ) x = ( width  - gtk3rwin->core.rbuf.ximg->width  ) * 0.5f;
    if ( height > gtk3rwin->core.rbuf.ximg->height ) y = ( height - gtk3rwin->core.rbuf.ximg->height ) * 0.5f;

    /*pthread_mutex_lock ( &lock );*/
    XShmPutImage ( gtk3rwin->core.rbuf.dis,
                   gtk3rwin->core.rbuf.win, 
                   gtk3rwin->core.rbuf.gc, 
                   gtk3rwin->core.rbuf.ximg,
                   0x00, 0x00,
                   x, y,
                   gtk3rwin->core.rbuf.ximg->width, 
                   gtk3rwin->core.rbuf.ximg->height, False );

    /*** must be called or else expect jerky effects ***/
    XSync ( gtk3rwin->core.rbuf.dis, 0 ); 
    XFlush ( gtk3rwin->core.rbuf.dis );
    #endif
    #ifdef __MINGW32__
    if ( width  > gtk3rwin->core.rbuf.wimg->bi->bmiHeader.biWidth  ) x = ( width  - gtk3rwin->core.rbuf.wimg->bi->bmiHeader.biWidth  ) * 0.5f;
    if ( height > gtk3rwin->core.rbuf.wimg->bi->bmiHeader.biHeight ) y = ( height - gtk3rwin->core.rbuf.wimg->bi->bmiHeader.biHeight ) * 0.5f;

    SetDIBitsToDevice ( dc, x,                                 /* int XDest               */
                            y,                                 /* int YDest               */
                            gtk3rwin->core.rbuf.wimg->bi->bmiHeader.biWidth,  /* DWORD dwWidth           */
                            gtk3rwin->core.rbuf.wimg->bi->bmiHeader.biHeight, /* DWORD dwHeight          */
                            0x00,                              /* int XSrc                */
                            0x00,                              /* int YSrc                */
                            0x00,                              /* UINT uStartScan         */
                            gtk3rwin->core.rbuf.wimg->bi->bmiHeader.biHeight, /* UINT cScanLines         */
                            gtk3rwin->core.rbuf.wimg->data,                   /* const VOID *lpvBits     */
                            gtk3rwin->core.rbuf.wimg->bi,                     /* const BITMAPINFO *lpbmi */
                            gtk3rwin->core.rbuf.wimg->bi->bmiHeader.biClrUsed );

    ReleaseDC ( gtk3rwin->core.rbuf.hWnd, dc );

    #endif

    /*pthread_mutex_unlock ( &lock );*/
}

/******************************************************************************/
static void Map ( GtkWidget *widget, 
                  gpointer   user_data ) {
    GTK3G3DUIRENDERWINDOW *gtk3rwin = ( GTK3G3DUIRENDERWINDOW * ) user_data;
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3rwin->core.gui;
    G3DUI *gui = &gtk3gui->core;
    G3DUIRENDERPROCESS *rps = gtk3rwin->core.rps;
    Q3DSETTINGS *rsg = gui->currsg;
    G3DCAMERA *cam = g3dui_getMainViewCamera ( gui );


    if ( rps == NULL ) {
        GdkDisplay *gdkdpy = gtk_widget_get_display ( gtk3rwin->drawingArea );
        GdkWindow  *gdkwin = gtk_widget_get_window  ( gtk3rwin->drawingArea );
        Q3DFILTER *makepreview = NULL;
        Q3DFILTER *towindow = NULL;
#ifdef __linux__
        g3duirenderbuffer_init ( &gtk3rwin->core.rbuf, 
                                  gdk_x11_display_get_xdisplay ( gdkdpy ),
                                  gdk_x11_window_get_xid       ( gdkwin ),
                                  rsg->output.width,
                                  rsg->output.height );

        towindow = q3dfilter_toWindow_new ( gtk3rwin->core.rbuf.dis,
                                            gtk3rwin->core.rbuf.win,
                                            gtk3rwin->core.rbuf.gc,
                                            rsg->output.width,
                                            rsg->output.height,
                                            gtk3rwin->core.rbuf.ximg,
                                            0x00 );

#endif
#ifdef __MINGW32__
        HWND hWnd = GDK_WINDOW_HWND ( gtk_widget_get_window ( gtk3rwin->drawingArea ) );

        g3duirenderbuffer_init ( &gtk3rwin->core.rbuf, 
                                  hWnd,
                                  rsg->output.width,
                                  rsg->output.height );

        towindow = q3dfilter_toWindow_new ( gtk3rwin->core.rbuf.hWnd,
                                            rsg->output.width,
                                            rsg->output.height,
                                            gtk3rwin->core.rbuf.wimg,
                                            0x00 );
#endif

        /*** This filter is used for saving images ***/
        /*R3DFILTER *tobuf = r3dfilter_toBuffer_new ( rsg->output.width, 
                                                    rsg->output.height, 
                                                    rsg->output.depth, 
                                                    rsg->background.color );


        if ( ((G3DOBJECT*)cam)->flags & CAMERADOF ) {
            R3DFILTER *rdf = r3dfilter_dof_new ( rsg->output.width, 
                                                 rsg->output.height, 
                                                 cam->dof.noBlur, 
                                                 cam->dof.farBlur,
                                                 cam->dof.radius );
            list_append ( &lfilters, rdf );
        }
*/

        if ( rsg->flags & RENDERPREVIEW ) {
#ifdef TODO
            makepreview = q3dfilter_preview_new ( gui );
#endif
        }

#ifdef unusedQISS3D
        if ( rsg->flags & RENDERSAVE ) {
            if ( rsg->output.format == RENDERTOVIDEO ) {
                R3DFILTER *ffmpg;
                char buf[0x100];

                snprintf ( buf, 0x100, "%s.avi", rsg->output.outfile );

                ffmpg = r3dfilter_toFfmpeg_new ( 0x00, rsg->output.width,
                                                       rsg->output.height,
                                                       rsg->output.depth,
                                                       rsg->output.fps,
                                                       rsg->output.endframe - rsg->output.startframe,
#ifdef __MINGW32__
                                                      &gui->cvars,
#endif
                                                       buf,
                                                       sysinfo->ffmpegPath,
                                                       sysinfo->ffplayPath );

                /*** ffmpg is NULL is ffmpeg is not found ***/
                if ( ffmpg ) {
                    list_append ( &lfilters, ffmpg );
                } else {
                    GtkWidget *dialog = gtk_message_dialog_new ( NULL,
                                                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                                                 GTK_MESSAGE_ERROR,
                                                                 GTK_BUTTONS_CLOSE,
                                                                 "FFMpeg not found in PATH!" );
                    gtk_dialog_run (GTK_DIALOG (dialog));
                    gtk_widget_destroy (dialog);
                }
            }


            if ( rsg->output.format == RENDERTOIMAGE ) {
                R3DFILTER *toimg;
                char buf[0x1000];

                snprintf ( buf, 0x1000, "%s.jpg", rsg->output.outfile );

                if ( rsg->output.startframe != rsg->output.endframe ) {
                    toimg = q3dfilter_writeImage_new ( buf, 0x01 );
                } else {
                    toimg = q3dfilter_writeImage_new ( buf, 0x00 );
                }

                list_append ( &lfilters, toimg );
            }
        }
#endif
        rsg->input.sce      = gui->sce;
        rsg->input.cam      = cam;

        rsg->background.widthRatio = 1.0f;

        rsg->output.x1 = 0x00;
        rsg->output.x2 = rsg->output.width  - 0x01;
        rsg->output.y1 = 0x00;
        rsg->output.y2 = rsg->output.height  - 0x01;
        rsg->background.image = sysinfo->backgroundImage;

        gtk3_setHourGlass ( gtk3gui );

        if ( rsg->output.startframe != rsg->output.endframe ) {
           /*** refresh fFMpeg path in case the user just installed it ***/
           g3dsysinfo_findFFMpeg ( sysinfo );

           if ( sysinfo->ffmpegPath == NULL ) {
               GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
               GtkMessageDialog *dialog;

               dialog = gtk_message_dialog_new ( NULL,
                                                 flags,
                                                 GTK_MESSAGE_ERROR,
                                                 GTK_BUTTONS_CLOSE,
                                                 "FFMpeg is needed to render to a video. Please install it and try again." );
               gtk_dialog_run ( GTK_DIALOG ( dialog ) );
               gtk_widget_destroy ( dialog );
           } else {
                gtk3rwin->core.rps = g3dui_render_q3d ( gui, 
                                                        rsg,
                                                        towindow,
                                                        gui->toframe,
                                                        gtk3rwin->core.tostatus,
                                                        makepreview,
                                                        cam,
                                                        rsg->output.startframe,
                                                        ( uint64_t ) widget,
                                                        0x01,
                                                        0x00 );
            }
        } else {
            gtk3rwin->core.rps = g3dui_render_q3d ( gui,
                                                    rsg,
                                                    towindow,
                                                    gui->toframe,
                                                    gtk3rwin->core.tostatus,
                                                    makepreview,
                                                    cam,
                                                    rsg->output.startframe,
                                                    ( uint64_t ) widget,
                                                    0x00,
                                                    0x00 );
        }

        gtk3_unsetHourGlass ( gtk3gui );
    }
}

/******************************************************************************/
static void Unmap ( GtkWidget *widget, 
                    gpointer   user_data ) {
    GTK3G3DUIRENDERWINDOW *grw = ( GTK3G3DUIRENDERWINDOW * ) user_data;
}


/******************************************************************************/
static void
gtk3_g3duirenderwindow_createDrawingArea ( GTK3G3DUIRENDERWINDOW *gtk3rwin ) {
    GtkWidget *scr = ui_gtk_scrolled_window_new ( CLASS_MAIN, NULL, NULL );
    GtkWidget *drw = ui_gtk_drawing_area_new ( CLASS_MAIN );

    gtk3rwin->scrolledWindow = scr;
    gtk3rwin->drawingArea    = drw;

    gtk_layout_put ( GTK_LAYOUT(gtk3rwin->layout), scr, 0, 0 );

    gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW(scr),
                                     GTK_POLICY_AUTOMATIC,
                                     GTK_POLICY_AUTOMATIC );

    /*** prevents erasing the background ***/
    gtk_widget_set_double_buffered (GTK_WIDGET (drw), FALSE);
    gtk_widget_set_app_paintable   (GTK_WIDGET (drw), TRUE);

    g_signal_connect ( G_OBJECT (drw), "draw" , G_CALLBACK (Draw) , gtk3rwin );
    g_signal_connect ( G_OBJECT (drw), "unmap", G_CALLBACK (Unmap), gtk3rwin );
    /** We use the map signal because we need drawable surface to be created **/
    g_signal_connect ( G_OBJECT (drw), "map"  , G_CALLBACK (Map)  , gtk3rwin );

/*#if GTK_CHECK_VERSION(3,8,0)*/
    gtk_container_add ( GTK_CONTAINER(scr), drw );
/*#else
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(scr), drw );
#endif*/

    gtk_widget_show ( drw );
    gtk_widget_show ( scr );


    return drw;
}

/******************************************************************************/
static void 
gtk3_g3duirenderwindow_createStatusBar ( GTK3G3DUIRENDERWINDOW *gtk3rwin ) {
    GtkStatusbar *bar = ui_gtk_statusbar_new ( CLASS_MAIN );
    GTK3G3DUI *gtk3gui = ( gtk3rwin->core.gui );
    G3DUI *gui = ( G3DUI * ) gtk3rwin->core.gui;
    Q3DSETTINGS *rsg = gui->currsg;

    gtk3rwin->statusBar = bar;

    gtk_layout_put ( GTK_LAYOUT(gtk3rwin->layout), bar, 0, 0 );

    gtk_widget_show ( bar );

    gtk3rwin->core.tostatus = q3dfilter_toStatusBar_new ( bar, rsg->output.endframe );


    return bar;
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3G3DUIRENDERWINDOW *gtk3rwin = ( GTK3G3DUIRENDERWINDOW * ) user_data;

    gtk3_g3duirenderwindow_updateMenuBar ( gtk3rwin );

    /*** MANDATORY !!! we allow only 1 rendering window ***/
    gtk3rwin->core.gui->renderWindow = &gtk3rwin->core;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIRENDERWINDOW *gtk3rwin = ( GTK3G3DUIRENDERWINDOW * ) user_data;
#ifdef __linux__
    /*XSync  ( gtk3rwin->core.rbuf.dis, 0 );
    XFlush ( gtk3rwin->core.rbuf.dis );*/
#endif
#ifdef __MINGW32__
    /*** nothing here ***/
#endif

    if ( gtk3rwin->core.rps ) {
        if ( gtk3rwin->core.rps->qjob ) {
            if ( gtk3rwin->core.rps->qjob->running ) {
                /*** tell the running thread thread to free the resources ***/
                gtk3rwin->core.rps->qjob->flags |= JOBFREEONCOMPLETION;

                q3djob_end ( gtk3rwin->core.rps->qjob );
            } else {
                /*** if the thread is not running anymore, free the resources ***/
                q3djob_free ( gtk3rwin->core.rps->qjob );

                /*g3duirenderbuffer_clear ( &gtk3rwin->core.rbuf );*/
            }
        }
    }

    g3duirenderbuffer_clear ( &gtk3rwin->core.rbuf );

    q3dfilter_free ( gtk3rwin->core.tostatus );

    /*** MANDATORY !!! we allow only 1 rendering window ***/
    gtk3rwin->core.gui->renderWindow = NULL;

    free ( gtk3rwin );
}

/******************************************************************************/
void gtk3_g3duirenderwindow_updateMenuBar ( GTK3G3DUIRENDERWINDOW *gtk3rwin ) {
    gtk3_g3duimenu_update_r ( ( GTK3G3DUIMENU * ) gtk3rwin->core.menuBar );
}

/******************************************************************************/
static void gtk3_g3duirenderwindow_createMenuBar ( GTK3G3DUIRENDERWINDOW *gtk3rwin ) {
    GTK3G3DUIMENU *gtk3menu;

    gtk3menu = gtk3_g3duimenu_parse_r ( g3duimenu_getRenderWindowMenuNode ( ),
                                        gtk3rwin->core.gui,
                                        gtk3rwin );

    gtk_layout_put ( GTK_LAYOUT(gtk3rwin->layout), gtk3menu->menu, 0, 0 );



    gtk3rwin->core.menuBar = ( G3DUIMENU * ) gtk3menu;
}

/******************************************************************************/
void gtk3_g3duirenderwindow_resize ( GTK3G3DUIRENDERWINDOW *gtk3rwin,
                                     uint32_t               width,
                                     uint32_t               height ) {
    if ( gtk_widget_get_realized ( GTK_WIDGET ( gtk3rwin->layout ) ) ) {
        GdkRectangle gdkrec;

        g3duirenderwindow_resize ( &gtk3rwin->core, 
                                    width, 
                                    height );

        if ( gtk3rwin->core.menuBar ) {
            GTK3G3DUIMENU *gtk3menu = ( GTK3G3DUIMENU * ) gtk3rwin->core.menuBar;

            g3duirectangle_toGdkRectangle ( &gtk3rwin->core.menurec, &gdkrec );

            gtk_layout_move ( gtk3rwin->layout,
                              gtk3menu->menu, 
                              gdkrec.x,
                              gdkrec.y );

            gtk_widget_set_size_request ( gtk3menu->menu,
                                          gdkrec.width,
                                          gdkrec.height );
        }

        if ( gtk3rwin->scrolledWindow ) {
            g3duirectangle_toGdkRectangle ( &gtk3rwin->core.scrrec, &gdkrec );

            gtk_layout_move ( gtk3rwin->layout,
                              gtk3rwin->scrolledWindow, 
                              gdkrec.x,
                              gdkrec.y );

            gtk_widget_set_size_request ( gtk3rwin->scrolledWindow,
                                          gdkrec.width,
                                          gdkrec.height );
        }

        if ( gtk3rwin->drawingArea ) {
            float zoomfactor = /*gtk3rwin->core.zoomFactor*/1.0f;

            gtk_widget_set_size_request ( gtk3rwin->scrolledWindow,
                                          gdkrec.width  * zoomfactor,
                                          gdkrec.height * zoomfactor );
        }

        if ( gtk3rwin->statusBar ) {
            g3duirectangle_toGdkRectangle ( &gtk3rwin->core.statusrec, &gdkrec );

            gtk_layout_move ( gtk3rwin->layout,
                              gtk3rwin->statusBar, 
                              gdkrec.x,
                              gdkrec.y );

            gtk_widget_set_size_request ( gtk3rwin->statusBar,
                                          gdkrec.width,
                                          gdkrec.height );
        }

        if ( gtk3rwin->drawingArea ) {
            g3duirectangle_toGdkRectangle ( &gtk3rwin->core.arearec, &gdkrec );

            gtk_widget_set_size_request ( gtk3rwin->drawingArea,
                                          gdkrec.width,
                                          gdkrec.height );
        }
    }
}

/******************************************************************************/
static void Configure ( GtkWidget *widget, 
                        GdkEvent  *event, 
                        gpointer   user_data ) {
    GTK3G3DUIRENDERWINDOW *gtk3rwin = ( GTK3G3DUIRENDERWINDOW * ) user_data;

    gtk3_g3duirenderwindow_resize ( gtk3rwin,
                                    event->configure.width,
                                    event->configure.height );
}

/******************************************************************************/
GTK3G3DUIRENDERWINDOW *gtk3_g3duirenderwindow_create ( GtkWindow *parent, 
                                                       GTK3G3DUI *gtk3gui,
                                                       char      *name ) {
    GTK3G3DUIRENDERWINDOW *gtk3rwin = gtk3_g3duirenderwindow_new ( gtk3gui );
    GtkLayout *layout = ui_gtk_layout_new ( CLASS_MAIN, NULL, NULL );

    gtk3rwin->topWin = parent;
    gtk3rwin->layout = layout;

    gtk_widget_set_name ( layout, name );

    gtk_container_add ( GTK_CONTAINER(parent), layout );

    gtk3_g3duirenderwindow_createMenuBar ( gtk3rwin );
    gtk3_g3duirenderwindow_createStatusBar ( gtk3rwin );
    gtk3_g3duirenderwindow_createDrawingArea ( gtk3rwin );
/*
    createRenderWindowDrawingArea ( gtk3rwin );
    createStatusBar               ( gtk3rwin );
*/
    g_signal_connect (G_OBJECT(layout), "realize", G_CALLBACK (Realize), gtk3rwin );
    g_signal_connect (G_OBJECT(layout), "destroy", G_CALLBACK (Destroy), gtk3rwin );

    gtk_widget_add_events ( GTK_WIDGET(parent), GDK_CONFIGURE );

    g_signal_connect ( G_OBJECT (parent), "configure-event", G_CALLBACK (Configure), gtk3rwin );

    gtk_widget_show ( layout );

    return gtk3rwin;
}

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

#ifdef unused

/******************************************************************************/
static GTK3G3DUIRENDERWINDOW *g3duirenderwindow_new ( G3DUI *gui ) {
    uint32_t size = sizeof ( GTK3G3DUIRENDERWINDOW );
    GTK3G3DUIRENDERWINDOW *grw = ( GTK3G3DUIRENDERWINDOW * ) calloc ( 0x01, size );

    if ( grw == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    grw->grp.gui = gui;


    return grw;
}

/******************************************************************************/
static GtkWidget *window_get_area ( GtkWidget * );

/******************************************************************************/
void Draw ( GtkWidget *widget, cairo_t *cr, gpointer user_data ) {
    GTK3G3DUIRENDERWINDOW *grw = ( GTK3G3DUIRENDERWINDOW * ) user_data;
    GdkDisplay *gdkdpy   = gtk_widget_get_display ( widget );
    GdkWindow  *gdkwin   = gtk_widget_get_window  ( widget );
    G3DUI *gui = ( G3DUI * ) grw->grp.gui;
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
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    #endif
    #ifdef __MINGW32__
    HDC dc = GetDC ( grw->rbuf.hWnd );
    #endif

    /*** Tell cairo to shut the F*** up ***/
    GdkRectangle arec;

    arec.x = arec.y = 0;
    arec.width = arec.height = 1;

    gdk_window_invalidate_rect ( gtk_widget_get_window ( widget ), &arec, FALSE );
    /*************************************/

    #ifdef __linux__
    if ( width  > grw->rbuf.ximg->width  ) x = ( width  - grw->rbuf.ximg->width  ) * 0.5f;
    if ( height > grw->rbuf.ximg->height ) y = ( height - grw->rbuf.ximg->height ) * 0.5f;

    /*pthread_mutex_lock ( &lock );*/
    XShmPutImage ( grw->rbuf.dis,
                   grw->rbuf.win, 
                   grw->rbuf.gc, 
                   grw->rbuf.ximg,
                   0x00, 0x00,
                   x, y,
                   grw->rbuf.ximg->width, 
                   grw->rbuf.ximg->height, False );
    /*** must be called or else expect jerky effects ***/
    XSync ( grw->rbuf.dis, 0 ); 
    XFlush ( grw->rbuf.dis );
    #endif
    #ifdef __MINGW32__
    if ( width  > grw->rbuf.wimg->bi->bmiHeader.biWidth  ) x = ( width  - grw->rbuf.wimg->bi->bmiHeader.biWidth  ) * 0.5f;
    if ( height > grw->rbuf.wimg->bi->bmiHeader.biHeight ) y = ( height - grw->rbuf.wimg->bi->bmiHeader.biHeight ) * 0.5f;

    SetDIBitsToDevice ( dc, x,                                 /* int XDest               */
                            y,                                 /* int YDest               */
                            grw->rbuf.wimg->bi->bmiHeader.biWidth,  /* DWORD dwWidth           */
                            grw->rbuf.wimg->bi->bmiHeader.biHeight, /* DWORD dwHeight          */
                            0x00,                              /* int XSrc                */
                            0x00,                              /* int YSrc                */
                            0x00,                              /* UINT uStartScan         */
                            grw->rbuf.wimg->bi->bmiHeader.biHeight, /* UINT cScanLines         */
                            grw->rbuf.wimg->data,                   /* const VOID *lpvBits     */
                            grw->rbuf.wimg->bi,                     /* const BITMAPINFO *lpbmi */
                            grw->rbuf.wimg->bi->bmiHeader.biClrUsed );

    ReleaseDC ( grw->rbuf.hWnd, dc );

    #endif
    /*pthread_mutex_unlock ( &lock );*/
}

/******************************************************************************/
uint32_t filtertostatusbar_getStatus ( Q3DFILTER *fil ) {
    FILTERTOSTATUSBAR *tsb = ( FILTERTOSTATUSBAR * ) fil->data;

    return tsb->done;
}

/******************************************************************************/
uint32_t filtertostatusbar_draw ( Q3DFILTER     *fil, 
                                  Q3DJOB        *qjob,
                                  uint32_t       cpuID, 
                                  float          frameID,
                                  unsigned char *img, 
                                  uint32_t       from, 
                                  uint32_t       to, 
                                  uint32_t       depth, 
                                  uint32_t       width ) {
    FILTERTOSTATUSBAR *tsb = ( FILTERTOSTATUSBAR * ) fil->data;
    guint cont = gtk_statusbar_get_context_id ( tsb->widget, "context" );
    static char str[100];

    /*** When called from "filter render before" event ***/
    if ( ( from == 0x00 ) &&
         ( to   == 0x00 ) ) {
        snprintf ( str, 100, "Rendering frame %.2f (%.2f%%)", frameID,
                                                              ( frameID - qjob->qrsg->output.startframe ) / ( qjob->qrsg->output.endframe - qjob->qrsg->output.startframe ) * 100.0f );
    } else {
        /*** when called from "filter render image" event ***/
        if ( ( int ) tsb->lastFrame == ( int ) frameID ) {
            snprintf ( str, 100, "Done (100%%)" );

            tsb->done = 0xFFFFFFFF;
        }
    }

    gtk_statusbar_push ( tsb->widget, cont, str );

    return 0x00;
}

/******************************************************************************/
void filtertostatusbar_free ( Q3DFILTER *fil ) {
    FILTERTOSTATUSBAR *tsb = ( FILTERTOSTATUSBAR * ) fil->data;

    free ( tsb );
}

/******************************************************************************/
/*** This filter is declared in the g3dui layer because of GtkWidget struct***/
Q3DFILTER *q3dfilter_toStatusBar_new ( GtkWidget *widget, 
                                       float      lastFrame ) {
    FILTERTOSTATUSBAR *tsb = calloc ( 0x01, sizeof ( FILTERTOSTATUSBAR ) );

    Q3DFILTER *fil = q3dfilter_new ( FILTERBEFORE | FILTERIMAGE,
                                     TOSTATUSBARFILTERNAME,
                                     filtertostatusbar_draw,
                                     filtertostatusbar_free, 
                                     tsb );

    tsb->widget = widget;
    tsb->lastFrame = lastFrame;

    return fil;
}

/******************************************************************************/
static void Map ( GtkWidget *widget, 
                  gpointer   user_data ) {
    GTK3G3DUIRENDERWINDOW *grw = ( GTK3G3DUIRENDERWINDOW * ) user_data;
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    G3DUI *gui = grw->grp.gui;
    G3DUIGTK3   *ggt  = ( G3DUIGTK3 * ) gui->toolkit_data;
    G3DUIRENDERPROCESS *rps = grw->rps;
    Q3DSETTINGS *rsg = gui->currsg;
    /*rsg->width  = gtk_widget_get_allocated_width  ( widget );
    rsg->height = gtk_widget_get_allocated_height ( widget );*/
    G3DCAMERA *cam = g3dui_getMainViewCamera ( gui );

    if ( rps == NULL ) {
        g3duirenderbuffer_init ( &grw->rbuf, 
                                  grw->drawingArea );
        Q3DFILTER *makepreview = NULL;
#ifdef __linux__
        Q3DFILTER *towindow = q3dfilter_toWindow_new ( grw->rbuf.dis,
                                                       grw->rbuf.win,
                                                       grw->rbuf.gc,
                                                       grw->rbuf.ximg,
                                                       0x00 );
#endif
#ifdef __MINGW32__
        Q3DFILTER *towindow = q3dfilter_toWindow_new ( grw->rbuf.hWnd,
                                                       grw->rbuf.wimg,
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
            makepreview = q3dfilter_preview_new ( gui );
        }

        grw->tostatus = q3dfilter_toStatusBar_new ( getChild ( gtk_widget_get_toplevel ( widget ), RENDERWINDOWSTATUSBARNAME ), rsg->output.endframe );

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

        g3dui_setHourGlass ( gui );

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
                grw->rps = common_g3dui_render_q3d ( gui, 
                                                     rsg,
                                                     towindow,
                                                     gui->toframe,
                                                     grw->tostatus,
                                                     makepreview,
                                                     cam,
                                                     rsg->output.startframe,
                                                     ( uint64_t ) widget,
                                                     0x01,
                                                     0x00 );
            }
        } else {
            grw->rps = common_g3dui_render_q3d ( gui,
                                                 rsg,
                                                 towindow,
                                                 gui->toframe,
                                                 grw->tostatus,
                                                 makepreview,
                                                 cam,
                                                 rsg->output.startframe,
                                                 ( uint64_t ) widget,
                                                 0x00,
                                                 0x00 );
        }

        g3dui_unsetHourGlass ( gui );
    }
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3G3DUIRENDERWINDOW *grw = ( GTK3G3DUIRENDERWINDOW * ) user_data;

}

/******************************************************************************/
static void Pause ( GtkWidget    *widget, 
                    GdkRectangle *allocation,
                    gpointer      user_data ) {
    GTK3G3DUIRENDERWINDOW *grw = ( GTK3G3DUIRENDERWINDOW * ) user_data;

}

/******************************************************************************/
static void Unmap ( GtkWidget *widget, 
                    gpointer   user_data ) {
    GTK3G3DUIRENDERWINDOW *grw = ( GTK3G3DUIRENDERWINDOW * ) user_data;
}

/******************************************************************************/
static void createRenderWindowDrawingArea ( GtkWidget        *parent, 
                                            GTK3G3DUIRENDERWINDOW *grw,
                                            char             *name,
                                            gint              x,
                                            gint              y,
                                            gint              width,
                                            gint              height ) {
    GdkRectangle scrrec = { 0, 0, width + 32, height + 48 };
    GdkRectangle drwrec = { 0, 0, 0x120, 0x120  };
    GtkWidget *scr, *drw;


    scr = gtk_scrolled_window_new ( NULL, NULL );

    gtk_widget_set_name ( scr, name );

    gtk_widget_set_size_request ( scr, scrrec.width, scrrec.height );

    gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW(scr),
                                     GTK_POLICY_AUTOMATIC,
                                     GTK_POLICY_AUTOMATIC );

    gtk_fixed_put ( GTK_FIXED(parent), scr, x, y );



    /*** Drawing area within the Scrolled Window ***/
    drw = gtk_drawing_area_new ( );

    /*** For keyboard inputs ***/
    gtk_widget_set_can_focus ( drw, TRUE );

    /*** prevents erasing the background ***/
    gtk_widget_set_double_buffered (GTK_WIDGET (drw), FALSE);
    gtk_widget_set_app_paintable (GTK_WIDGET (drw), TRUE);

    /*** Drawing area does not receive mous events by defaults ***/
    gtk_widget_set_events ( drw, gtk_widget_get_events ( drw )  |
                                 GDK_KEY_PRESS_MASK             |
                                 GDK_KEY_RELEASE_MASK           |
                                 GDK_BUTTON_PRESS_MASK          |
                                 GDK_BUTTON_RELEASE_MASK        |
                                 GDK_POINTER_MOTION_MASK        |
                                 GDK_POINTER_MOTION_HINT_MASK );

    g_signal_connect ( G_OBJECT (drw), "size-allocate", G_CALLBACK (Pause  ), grw );
    g_signal_connect ( G_OBJECT (drw), "realize"      , G_CALLBACK (Realize), grw );
    g_signal_connect ( G_OBJECT (drw), "draw"         , G_CALLBACK (Draw   ), grw );
    g_signal_connect ( G_OBJECT (drw), "unmap"        , G_CALLBACK (Unmap), grw );
    /** We use the map signal because we need drawable surface to be created **/
    g_signal_connect ( G_OBJECT (drw), "map"          , G_CALLBACK (Map)    , grw );

    gtk_widget_set_size_request ( drw, 
                                  grw->grp.gui->currsg->output.width, 
                                  grw->grp.gui->currsg->output.height );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_container_add ( GTK_CONTAINER(scr), drw );
#else
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(scr), drw );
#endif

    gtk_widget_show ( drw );
    gtk_widget_show ( scr );

    grw->scrolledWindow = scr;
    grw->drawingArea = drw;

    return drw;
}

/******************************************************************************/
static void createStatusBar ( GtkWidget        *parent, 
                              GTK3G3DUIRENDERWINDOW *grw,
                              char             *name,
                              gint             x, 
                              gint             y,
                              gint             width,
                              gint             height ) {
    GtkWidget *bar = gtk_statusbar_new ( );

    gtk_widget_set_name ( bar, name );

    gtk_widget_set_size_request ( bar, width, height );

    gtk_fixed_put ( GTK_FIXED(parent), bar, x, y );

    gtk_widget_show ( bar );

    grw->statusBar = bar;


    return bar;
}

/******************************************************************************/
static void wResize ( GtkWidget    *widget, 
                      GdkRectangle *allocation, 
                      gpointer      user_data ) {
    GTK3G3DUIRENDERWINDOW *grw = ( GTK3G3DUIRENDERWINDOW * ) user_data;
    GdkRectangle gdkrec;

    gdkrec.x      = 0x00;
    gdkrec.y      = 0x00;
    gdkrec.width  = allocation->width;
    gdkrec.height = 0x20;
    gtk_widget_size_allocate ( grw->menuBar, &gdkrec );

    gdkrec.x      = 0x00;
    gdkrec.y      = 0x20;
    gdkrec.width  = allocation->width;
    gdkrec.height = allocation->height - 0x40;
    gtk_widget_size_allocate ( grw->scrolledWindow, &gdkrec );

    gdkrec.x      = 0x00;
    gdkrec.y      = allocation->height - 0x20;
    gdkrec.width  = allocation->width;
    gdkrec.height = 0x20;
    gtk_widget_size_allocate ( grw->statusBar, &gdkrec );
}
#endif

/******************************************************************************/
static void Realize ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3G3DUIRENDERWINDOW *grw = ( GTK3G3DUIRENDERWINDOW * ) user_data;

}

/******************************************************************************/
static gboolean Destroy ( GtkWidget *widget,
                          gpointer   user_data ) {
    GTK3G3DUIRENDERWINDOW *gtk3rwin = ( GTK3G3DUIRENDERWINDOW * ) user_data;
#ifdef __linux__
    XSync  ( gtk3rwin->core.rbuf.dis, 0 );
    XFlush ( gtk3rwin->core.rbuf.dis );
#endif
#ifdef __MINGW32__
    /*** nothing here ***/
#endif

    if ( gtk3rwin->core.rps->qjob->running ) {
        /*** tell the running thread thread to free the resources ***/
        gtk3rwin->core.rps->qjob->flags |= JOBFREEONCOMPLETION;

        q3djob_end ( gtk3rwin->core.rps->qjob );
    } else {
        /*** if the thread is not running anymore, free the resources ***/
        q3djob_free ( gtk3rwin->core.rps->qjob );

        g3duirenderbuffer_clear ( &gtk3rwin->core.rbuf );
    }

    g3duirenderbuffer_clear ( &gtk3rwin->core.rbuf );

    q3dfilter_free ( gtk3rwin->tostatus );

    free ( gtk3rwin );
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
static void Configure ( GtkWidget *widget, 
                        GdkEvent  *event, 
                        gpointer   user_data ) {
    GTK3G3DUIRENDERWINDOW *gtk3rwin = ( GTK3G3DUIRENDERWINDOW * ) user_data;
    GdkRectangle gdkrec;

    if ( gtk_widget_get_realized ( GTK_WIDGET ( window ) ) ) {
        g3duirenderwindow_resize ( &gtk3rwin->core, 
                                    event->configure.width, 
                                    event->configure.height );

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

        if ( gtk3rwin->area ) {
            float zoomfactor = /*gtk3rwin->core.zoomFactor*/1.0f;

            gtk_widget_set_size_request ( gtk3rwin->scrolledWindow,
                                          gdkrec.width  * zoomfactor,
                                          gdkrec.height * zoomfactor );
        }
    }
}

/******************************************************************************/
GTK3G3DUIRENDERWINDOW *gtk3_g3duirenderwindow_create ( GtkWindow *parent, 
                                                       GTK3G3DUI *gtk3gui,
                                                       char      *name ) {
    GTK3G3DUIRENDERWINDOW *gtk3rwin = gtk3_g3duirenderwindow_new ( gtk3gui );
    GtkLayout *layout = ui_gtk_layout_new ( );

    gtk3rwin->topWin = parent;
    gtk3rwin->layout = layout;

    gtk_widget_set_name ( layout, name );

    gtk_container_add ( GTK_CONTAINER(parent), layout );

    createRenderWindowMenuBar     ( gtk3rwin );
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

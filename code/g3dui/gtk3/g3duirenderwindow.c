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
static GtkWidget *window_get_area ( GtkWidget * );

/******************************************************************************/
void Draw ( GtkWidget *widget, cairo_t *cr, gpointer user_data ) {
    GdkDisplay *gdkdpy   = gtk_widget_get_display ( widget );
    GdkWindow  *gdkwin   = gtk_widget_get_window  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, ( uint64_t ) widget );

    /* rps might not be allocated yet by the map signal of the renderwindow */
    if ( rps ) {
        Q3DFILTER *fil = rps->qjob->filters.towindow;

        if ( fil ) {
            FILTERTOWINDOW *ftw = ( FILTERTOWINDOW * ) fil->data;
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
            HDC dc = GetDC ( ftw->hWnd );
            #endif

            /*** Tell cairo to shut the F*** up ***/
            GdkRectangle arec;

            arec.x = arec.y = 0;
            arec.width = arec.height = 1;

            gdk_window_invalidate_rect ( gtk_widget_get_window ( widget ), &arec, FALSE );
            /*************************************/

            #ifdef __linux__
            if ( width  > ftw->ximg->width  ) x = ( width  - ftw->ximg->width  ) * 0.5f;
            if ( height > ftw->ximg->height ) y = ( height - ftw->ximg->height ) * 0.5f;

            /*pthread_mutex_lock ( &lock );*/
            XShmPutImage ( ftw->dis, ftw->win, 
                                     ftw->gc, 
                                     ftw->ximg,
                                     0x00, 0x00,
                                     x, y,
                                     ftw->ximg->width, 
                                     ftw->ximg->height, False );
            /*** must be called or else expect jerky effects ***/
            XSync ( ftw->dis, 0 ); 
            XFlush ( ftw->dis );
            #endif
            #ifdef __MINGW32__
            if ( width  > ftw->wimg->bi->bmiHeader.biWidth  ) x = ( width  - ftw->wimg->bi->bmiHeader.biWidth  ) * 0.5f;
            if ( height > ftw->wimg->bi->bmiHeader.biHeight ) y = ( height - ftw->wimg->bi->bmiHeader.biHeight ) * 0.5f;

            SetDIBitsToDevice ( dc, x,                                 /* int XDest               */
                                    y,                                 /* int YDest               */
                                    ftw->wimg->bi->bmiHeader.biWidth,  /* DWORD dwWidth           */
                                    ftw->wimg->bi->bmiHeader.biHeight, /* DWORD dwHeight          */
                                    0x00,                              /* int XSrc                */
                                    0x00,                              /* int YSrc                */
                                    0x00,                              /* UINT uStartScan         */
                                    ftw->wimg->bi->bmiHeader.biHeight, /* UINT cScanLines         */
                                    ftw->wimg->data,                   /* const VOID *lpvBits     */
                                    ftw->wimg->bi,                     /* const BITMAPINFO *lpbmi */
                                    ftw->wimg->bi->bmiHeader.biClrUsed );

            ReleaseDC ( ftw->hWnd, dc );

            #endif
            /*pthread_mutex_unlock ( &lock );*/
        }
    }
}

/******************************************************************************/
uint32_t filtertostatusbar_draw ( Q3DFILTER *fil, Q3DJOB *qjob,
                                                  float frameID,
                                                  unsigned char *img, 
                                                  uint32_t from, 
                                                  uint32_t to, 
                                                  uint32_t depth, 
                                                  uint32_t width ) {
    FILTERTOSTATUSBAR *tsb = ( FILTERTOSTATUSBAR * ) fil->data;
    guint cont = gtk_statusbar_get_context_id ( tsb->widget, "context" );
    static char str[100];

    /*** When called from "filter render before" event ***/
    if ( ( from == 0x00 ) &&
         ( to   == 0x00 ) ) {
        snprintf ( str, 100, "Rendering frame %.2f (%.2f%%)", frameID,
                                                              frameID / ( qjob->qrsg->output.endframe - qjob->qrsg->output.startframe ) * 100.0f );
    } else {
        /*** when called from "filter render image" event ***/
        if ( ( int ) tsb->lastFrame == ( int ) frameID ) {
            snprintf ( str, 100, "Done (100%%)" );
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
Q3DFILTER *q3dfilter_toStatusBar_new ( GtkWidget *widget, float lastFrame ) {
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
static void Map ( GtkWidget *widget, gpointer user_data ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3   *ggt  = ( G3DUIGTK3 * ) gui->toolkit_data;
    G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, ( uint64_t ) widget );
    Q3DSETTINGS *rsg = gui->currsg;
    GdkDisplay *gdkdpy   = gtk_widget_get_display ( ggt->curogl );
    GdkWindow  *gdkwin   = gtk_widget_get_window  ( ggt->curogl );
    /*Display    *dis      = gdk_x11_display_get_xdisplay ( gdkdpy );*/
    /*Window      win      = gdk_x11_window_get_xid ( gdkwin );*/
    /*rsg->width  = gtk_widget_get_allocated_width  ( widget );
    rsg->height = gtk_widget_get_allocated_height ( widget );*/
    G3DCAMERA *cam = g3dui_getMainViewCamera ( gui );

    if ( rps == NULL ) {

        /*** this filter is used for displaying ***/
        Q3DFILTER *towin = q3dfilter_toGtkWidget_new ( widget, 0x00 );
            /*** this filter tells the engine to go to the next frame ***/
            Q3DFILTER *toframe = q3dfilter_gotoframe_new ( gui );

        /*** This filter is used for saving images ***/
        /*R3DFILTER *tobuf = r3dfilter_toBuffer_new ( rsg->output.width, 
                                                    rsg->output.height, 
                                                    rsg->output.depth, 
                                                    rsg->background.color );

        list_append ( &lfilters, r3dfilter_toStatusBar_new(getChild(gtk_widget_get_toplevel (widget),RENDERWINDOWSTATUSBARNAME), rsg->output.endframe ) );

        if (   ( rsg->flags & ENABLEMOTIONBLUR ) && 
             ( ( rsg->flags & RENDERPREVIEW    ) == 0x00 ) ) {
            R3DFILTER *blur;

            if ( rsg->flags & VECTORMOTIONBLUR ) {
                blur = r3dfilter_VectorMotionBlur_new ( rsg->output.width, 
                                                        rsg->output.height,
                                              ( float ) rsg->motionBlur.strength / 100,
                                                        rsg->motionBlur.vMotionBlurSamples,
                                                        rsg->motionBlur.vMotionBlurSubSamplingRate );

            }

            if ( rsg->flags & SCENEMOTIONBLUR ) {
                blur = r3dfilter_MotionBlur_new ( rsg->output.width, 
                                                  rsg->output.height,
                                                  rsg->output.depth, 
                                                  rsg->motionBlur.iterations );
            }

            if ( blur ) list_append ( &lfilters, blur );
        }

        if ( ((G3DOBJECT*)cam)->flags & CAMERADOF ) {
            R3DFILTER *rdf = r3dfilter_dof_new ( rsg->output.width, 
                                                 rsg->output.height, 
                                                 cam->dof.noBlur, 
                                                 cam->dof.farBlur,
                                                 cam->dof.radius );
            list_append ( &lfilters, rdf );
        }

        if ( rsg->flags & RENDERPREVIEW ) {
            list_append ( &lfilters, r3dfilter_preview_new ( gui ) );
        }*/

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
            /*** force starting at the first frame. **/
            /*** -1 is because the gotoframe filter will add 1 further. ***/
            /*** this should probably be changed ***/
           /* COMMENTED OUT: now done by g3drenderprocess */
            /*tofrm->draw ( tofrm, NULL, gui->currsg->startframe - 1, NULL, 0, 0, 0, 0 );*/

            rps = common_g3dui_render_q3d ( gui, 
                                            rsg,
                                            towin,
                                            toframe,
                                            cam,
                                            rsg->output.startframe,
                                            ( uint64_t ) widget,
                                            0x01 );
        } else {
            rps = common_g3dui_render_q3d ( gui,
                                            rsg,
                                            towin,
                                            toframe,
                                            cam,
                                            rsg->output.startframe,
                                            ( uint64_t ) widget,
                                            0x00 );
        }

        g3dui_unsetHourGlass ( gui );
    }
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

}

/******************************************************************************/
static void Pause ( GtkWidget *widget, GdkRectangle *allocation,
                                       gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

}

/******************************************************************************/
static void Unmap ( GtkWidget *widget, gpointer user_data ) {
/*static gboolean Destroy ( GtkWidget *widget, GdkEvent *event,
                                             gpointer user_data ) {*/
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, ( uint64_t ) widget );

    /*** cancel renderprocess if any ***/
    common_g3dui_cancelRenderByID ( gui, ( uint64_t ) widget );

    if ( rps ) {
        g3duirenderprocess_free ( rps );

        list_remove ( &gui->lrps, rps );
    }

    /*return TRUE;*/
}

/******************************************************************************/
GtkWidget *createRenderWindowDrawingArea ( GtkWidget *parent, G3DUI *gui,
                                                              char *name,
                                                              gint x,
                                                              gint y,
                                                              gint width,
                                                              gint height ) {
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

    g_signal_connect ( G_OBJECT (drw), "size-allocate", G_CALLBACK (Pause  ), gui );
    g_signal_connect ( G_OBJECT (drw), "realize"      , G_CALLBACK (Realize), gui );
    g_signal_connect ( G_OBJECT (drw), "draw"         , G_CALLBACK (Draw   ), gui );
    g_signal_connect ( G_OBJECT (drw), "unmap", G_CALLBACK (Unmap), gui );
    /** We use the map signal because we need drawable surface to be created **/
    g_signal_connect ( G_OBJECT (drw), "map"          , G_CALLBACK (Map)    , gui );


    /*g_signal_connect ( G_OBJECT (drw), "motion_notify_event" , G_CALLBACK (objectlistarea_input), gui );
    g_signal_connect ( G_OBJECT (drw), "button_press_event"  , G_CALLBACK (objectlistarea_input), gui );
    g_signal_connect ( G_OBJECT (drw), "button_release_event", G_CALLBACK (objectlistarea_input), gui );
    g_signal_connect ( G_OBJECT (drw), "key_press_event"     , G_CALLBACK (objectlistarea_input), gui );
    g_signal_connect ( G_OBJECT (drw), "key_release_event"   , G_CALLBACK (objectlistarea_input), gui );*/

    gtk_widget_set_size_request ( drw, gui->currsg->output.width, 
                                       gui->currsg->output.height );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_container_add ( GTK_CONTAINER(scr), drw );
#else
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(scr), drw );
#endif

    gtk_widget_show ( drw );
    gtk_widget_show ( scr );


    return drw;
}

/******************************************************************************/
static void saveAsImageCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Save image ...",
                                           GTK_WINDOW(ggt->top),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_SAVE,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    gtk_file_chooser_set_do_overwrite_confirmation ( GTK_FILE_CHOOSER(dialog),
                                                     TRUE );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        char           *filename = gtk_file_chooser_get_filename ( chooser );
        /*** All widget have RPS as a user data ***/
        GtkWidget *area =  g_object_get_data ( G_OBJECT(widget), GTK3WIDGETDATA );
        G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, ( uint64_t ) area );

        if ( rps ) {
            g3duirenderprocess_savejpg ( rps, filename );
        }

        g_free    ( filename );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
static void exitWindowCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    GtkWidget *area =  g_object_get_data ( G_OBJECT(widget), GTK3WIDGETDATA );
    /*** Top level widget of a menu button is the popup widget. This is why ***/
    /*** we use the area widget to retrieve the toplevel window ***/
    GtkWidget *window = gtk_widget_get_toplevel ( area );

    gtk_widget_destroy ( window );
}

/******************************************************************************/
GtkWidget *createRenderWindowFileMenu ( GtkWidget *bar, G3DUI *gui,
                                                        char *name,
                                                        gint width ) {
    GtkWidget *menu = gtk_menu_new ( ), *btn;
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( "_File" );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_halign ( item, GTK_ALIGN_CENTER );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( bar ), item );

    btn = g3dui_addMenuButton    ( menu, gui, "Save As JPEG", width, G_CALLBACK(saveAsImageCbk) );

    g3dui_addMenuSeparator ( menu );
    g3dui_addMenuButton    ( menu, gui, "Close"        , width, G_CALLBACK(exitWindowCbk) );

    gtk_widget_show ( item );
    gtk_widget_show ( menu );


    return menu;
}

/******************************************************************************/
/** Store the drawing area pointer in the menu button. We need the are to   ***/
/** retrieve the G3DUIRENDERPROCESS with g3dui_getRenderProcessByID() ***/
static void menubar_set_area_r ( GtkWidget *widget, GtkWidget *area ) {
    if ( G_IS_OBJECT(widget) ) {
        g_object_set_data ( G_OBJECT(widget), GTK3WIDGETDATA, area );

        if ( GTK_IS_CONTAINER ( widget ) ) {
            GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

            while ( children ) {
                GtkWidget *child = ( GtkWidget * ) children->data;

                /*** recurse ***/
                menubar_set_area_r ( child, area );

                children = g_list_next ( children );
            }
        }

        if ( GTK_IS_MENU_ITEM ( widget ) ) {
            GtkWidget *submenu = gtk_menu_item_get_submenu ( GTK_MENU_ITEM(widget) );

            menubar_set_area_r ( submenu, area );
        }
    }
}

/******************************************************************************/
GtkWidget *createRenderWindowMenuBar ( GtkWidget *parent, G3DUI *gui,
                                                          char *name,
                                                          gint x,
                                                          gint y,
                                                          gint width,
                                                          gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_menu_bar_new ( );

    gtk_widget_set_name ( bar, name );

    gtk_widget_size_allocate ( bar, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), bar, x, y );

    createRenderWindowFileMenu ( bar, gui, "FileMenu", 60 );

    gtk_widget_show ( bar );


    return bar;
}

/******************************************************************************/
static GtkWidget *window_get_area ( GtkWidget *widget ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( GTK_IS_SCROLLED_WINDOW(child) ) {
            GtkWidget *viewport = gtk_bin_get_child ( GTK_BIN(child) );
                  /*** Watch out the GtkViewport container ***/
           return gtk_bin_get_child ( GTK_BIN(viewport) );
        }

        children = g_list_next ( children );
    }

    return NULL;
}

/******************************************************************************/
static GtkWidget *window_get_mbar ( GtkWidget *widget ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( GTK_IS_MENU_BAR(child) ) {

           return child;
        }

        children = g_list_next ( children );
    }

    return NULL;
}

/******************************************************************************/
static void wResize ( GtkWidget *widget, GdkRectangle *allocation, 
                                         gpointer user_data ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );
        GdkRectangle gdkrec;

        if ( GTK_IS_MENU_BAR(child) ) {
            gdkrec.x      = 0x00;
            gdkrec.y      = 0x00;
            gdkrec.width  = allocation->width;
            gdkrec.height = 0x20;

            /*if ( gtk_widget_get_has_window ( widget ) == 0x00 ) {
                gdkrec.x += allocation->x;
                gdkrec.y += allocation->y;
            }*/

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        if ( GTK_IS_SCROLLED_WINDOW(child) ) {
            gdkrec.x      = 0x00;
            gdkrec.y      = 0x20;
            gdkrec.width  = allocation->width;
            gdkrec.height = allocation->height - 0x40;

            /*if ( gtk_widget_get_has_window ( widget ) == 0x00 ) {
                gdkrec.x += allocation->x;
                gdkrec.y += allocation->y;
            }*/

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        if ( GTK_IS_STATUSBAR(child) ) {
            gdkrec.x      = 0x00;
            gdkrec.y      = allocation->height - 0x20;
            gdkrec.width  = allocation->width;
            gdkrec.height = 0x20;

            /*if ( gtk_widget_get_has_window ( widget ) == 0x00 ) {
                gdkrec.x += allocation->x;
                gdkrec.y += allocation->y;
            }*/

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        children = g_list_next ( children );
    }
}

/******************************************************************************/
static void wRealize ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *area = window_get_area ( widget );
    GtkWidget *mbar = window_get_mbar ( widget );

    menubar_set_area_r ( mbar, area );
}

/******************************************************************************/
GtkWidget *createRenderWindow ( GtkWidget *parent, G3DUI *gui,
                                                   char *name,
                                                   gint x,
                                                   gint y,
                                                   gint width,
                                                   gint height ) {
    GdkRectangle gdkrec = { 0, 0, width, height };
    GtkWidget *frm, *area, *mbar;

    /*if ( width  > 1024 ) gdkrec.width  = 1024;
    if ( height >  768 ) gdkrec.height = 768;*/

    frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    gtk_container_add ( GTK_CONTAINER(parent), frm );

    g_signal_connect (G_OBJECT(frm), "size-allocate", G_CALLBACK (wResize) , gui);
    g_signal_connect (G_OBJECT(frm), "realize"      , G_CALLBACK (wRealize), gui);

    createRenderWindowDrawingArea ( frm, gui, RENDERWINDOWMENUWORKAREANAME, 0, 32, width, height - 48 );
    createRenderWindowMenuBar     ( frm, gui, RENDERWINDOWMENUBARNAME     , 0,  0, width, 32 );
    createStatusBar               ( frm, gui, RENDERWINDOWSTATUSBARNAME   , 0,  height - 48, width, 16 );

    gtk_widget_show ( frm );


    return frm;
}

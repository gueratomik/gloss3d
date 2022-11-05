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
#include <xpm/translate_view.xpm>
#include <xpm/rotate_view.xpm>
#include <xpm/maximize_view.xpm>
#include <xpm/zoom_view.xpm>

/******************************************************************************/
void gdkevent_to_g3devent ( GdkEvent *gdkev, G3DEvent *g3dev ) {
    memset ( g3dev, 0x00, sizeof ( G3DEvent ) );

    switch ( gdkev->type ) {
        case GDK_BUTTON_PRESS : {
            GdkEventButton *gdkbev = ( GdkEventButton * ) gdkev;
            G3DButtonEvent *g3dbev = ( G3DButtonEvent * ) g3dev;

            g3dbev->type  = G3DButtonPress;
            g3dbev->x     = gdkbev->x;
            g3dbev->y     = gdkbev->y;

            g3dbev->button = gdkbev->button;

            if ( gdkbev->state & GDK_CONTROL_MASK ) g3dbev->state |= G3DControlMask;
            if ( gdkbev->state & GDK_SHIFT_MASK   ) g3dbev->state |= G3DShiftMask;

            /*if ( gdkbev->state & GDK_BUTTON1_MASK ) g3dbev->state |= G3DButton1Mask;
            if ( gdkbev->state & GDK_BUTTON2_MASK ) g3dbev->state |= G3DButton2Mask;
            if ( gdkbev->state & GDK_BUTTON3_MASK ) g3dbev->state |= G3DButton3Mask;*/

        } break;

        case GDK_BUTTON_RELEASE : {
            GdkEventButton *gdkbev = ( GdkEventButton * ) gdkev;
            G3DButtonEvent *g3dbev = ( G3DButtonEvent * ) g3dev;

            g3dbev->type = G3DButtonRelease;
            g3dbev->x    = gdkbev->x;
            g3dbev->y    = gdkbev->y;

            g3dbev->button = gdkbev->button;

            if ( gdkbev->state & GDK_CONTROL_MASK ) g3dbev->state |= G3DControlMask;
            if ( gdkbev->state & GDK_SHIFT_MASK   ) g3dbev->state |= G3DShiftMask;
            /*if ( gdkbev->state & GDK_BUTTON1_MASK ) g3dbev->state |= G3DButton1Mask;
            if ( gdkbev->state & GDK_BUTTON2_MASK ) g3dbev->state |= G3DButton2Mask;
            if ( gdkbev->state & GDK_BUTTON3_MASK ) g3dbev->state |= G3DButton3Mask;*/
        } break;

        case GDK_MOTION_NOTIFY : {
            GdkEventMotion *gdkmev = ( GdkEventMotion * ) gdkev;
            G3DMotionEvent *g3dmev = ( G3DMotionEvent * ) g3dev;

            g3dmev->type = G3DMotionNotify;
            g3dmev->x    = gdkmev->x;
            g3dmev->y    = gdkmev->y;

            if ( gdkmev->state & GDK_CONTROL_MASK ) g3dmev->state |= G3DControlMask;
            if ( gdkmev->state & GDK_SHIFT_MASK   ) g3dmev->state |= G3DShiftMask;
            if ( gdkmev->state & GDK_BUTTON1_MASK ) g3dmev->state |= G3DButton1Mask;
            if ( gdkmev->state & GDK_BUTTON2_MASK ) g3dmev->state |= G3DButton2Mask;
            if ( gdkmev->state & GDK_BUTTON3_MASK ) g3dmev->state |= G3DButton3Mask;
        } break;
    }
}

/******************************************************************************/
/*** for some unknown reason, the gtk_popup_menu does not position my menu ***/
/*** correctly on dual monitor configuration, so I had to make this trick ***/
void SetMenuPosition ( GtkMenu *menu, gint *x,
                                      gint *y,
                                      gboolean *push_in,
                                      gpointer user_data ) {
    GdkEventButton *bev = ( GdkEventButton  * ) user_data;
    GdkScreen *scr = gdk_screen_get_default ( );
    gint mx, my;
    int mon = gdk_screen_get_monitor_at_window ( scr, bev->window );

    gtk_menu_set_monitor ( menu, mon );

    gdk_window_get_origin ( bev->window, &mx, &my );
    /*gdk_window_get_device_position ( bev->window, bev->device,
                                                 &mx,
                                                 &my,
                                                  NULL );*/

    (*x) = mx + bev->x;
    (*y) = my + bev->y;
}

/******************************************************************************/
static void PostMenu ( GTK3G3DUI *gtk3gui,
                       GdkEvent  *event ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    GdkEventButton *bev      = ( GdkEventButton * ) event;
    G3DOBJECT      *selObj   = g3dscene_getSelectedObject ( gui->sce );
    GTK3G3DUIMENU *node = NULL;


    if ( gui->engine_flags & VIEWOBJECT ) {
        node = ( GTK3G3DUIMENU * ) gui->objectModeMenu;
    }

    if ( selObj ) {
        if ( gui->engine_flags & VIEWVERTEX ) {
            if ( selObj->type == G3DSPLINETYPE ) {
                node = ( GTK3G3DUIMENU * ) gui->vertexModeSplineMenu;
            }

            if ( selObj->type == G3DMESHTYPE ) {
                node = ( GTK3G3DUIMENU * ) gui->vertexModeMeshMenu;
            }

            if ( selObj->type == G3DMORPHERTYPE ) {
                node = ( GTK3G3DUIMENU * ) gui->vertexModeMorpherMenu;
            }
        }

        if ( gui->engine_flags & VIEWEDGE ) {
            if ( selObj->type == G3DMESHTYPE ) {
                node = ( GTK3G3DUIMENU * ) gui->edgeModeMeshMenu;
            }
        }

        if ( gui->engine_flags & VIEWFACE ) {
            if ( selObj->type == G3DMESHTYPE ) {
                node = ( GTK3G3DUIMENU * ) gui->faceModeMeshMenu;
            }
        }

        if ( gui->engine_flags & VIEWSCULPT ) {
            if ( selObj->type == G3DSUBDIVIDERTYPE ) {
                node = ( GTK3G3DUIMENU * ) gui->sculptModeMeshMenu;
            }
        }
    }

    if ( node ) {
        /*gtk_menu_popup_for_device ( GTK_MENU ( curmenu ), bev->device, 
                                                          NULL, 
                                                          NULL, 
                                                          NULL, 
                                                          NULL,
                                                          NULL,
                                                          bev->button,
                                                          gdk_event_get_time( ( GdkEvent * ) event ) );*/

        gtk_menu_popup ( GTK_MENU ( node->menu ), 
                         NULL, 
                         NULL,
                         SetMenuPosition,
                         bev,
                         bev->button,
                         gdk_event_get_time( ( GdkEvent * ) event ) );
    }
}


/******************************************************************************/
static gboolean inputGL ( GtkWidget *widget, 
                          GdkEvent  *gdkev, 
                          gpointer   user_data ) {
    GTK3G3DUIVIEW *gtk3view =  ( GTK3G3DUIVIEW * ) user_data;
    G3DUIVIEW     *view     = &gtk3view->core;
    G3DUI         *gui      = view->gui;
    GTK3G3DUI     *gtk3gui = ( GTK3G3DUI * ) gui;
    GTK3G3DUIQUAD *gtk3quad = ( GTK3G3DUIQUAD * ) gui->main->quad;

    if ( gui->currentView !=  ( G3DUIVIEW * ) gtk3view ) {
        gui->currentView = ( G3DUIVIEW * ) gtk3view;

        /*** foirce redrawing the frame around the current view ***/
        gtk_widget_queue_draw ( gtk3quad->layout );
    }

    if ( gdkev->type == GDK_BUTTON_PRESS ) {
        GdkEventButton *bev = ( GdkEventButton * ) gdkev;

        if ( bev->button == 3 ) {
            PostMenu ( gtk3gui, gdkev );

            return TRUE;
        }
    }

    switch ( gdkev->type ) {
        case GDK_KEY_PRESS : {
            GdkEventKey *kev = ( GdkEventKey * ) gdkev;

            switch ( kev->keyval ) {
                case GDK_KEY_Delete: {
                    uint64_t ret = g3dui_deleteSelection ( gui );

                    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
/*
                    if ( g3dui_deleteSelection ( gui ) == G3DERROR_OBJECT_REFERRED ) {
                        GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
                        GtkMessageDialog *dialog;

                        dialog = gtk_message_dialog_new ( NULL,
                                                          flags,
                                                          GTK_MESSAGE_ERROR,
                                                          GTK_BUTTONS_CLOSE,
                                                          "Could not remove referred object" );
                        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
                        gtk_widget_destroy ( dialog );
                    }
*/
                } break;

                case GDK_KEY_z: {
                    /*** undo ***/
                    if ( kev->state & GDK_CONTROL_MASK ) {
                        uint64_t ret = g3dui_undo ( gui );

                        gtk3_interpretUIReturnFlags ( gtk3gui, ret );
                    }
                } break;

                case GDK_KEY_y: {
                    /*** redo ***/
                    if ( kev->state & GDK_CONTROL_MASK ) {
                        uint64_t ret = g3dui_redo ( gui );

                        gtk3_interpretUIReturnFlags ( gtk3gui, ret );
                    }
                } break;

                case GDK_KEY_c: {
                    if ( kev->state & GDK_CONTROL_MASK ) {
                        g3dui_copySelection ( gui );
                    }
                } break;

                case GDK_KEY_v: {
                    if ( kev->state & GDK_CONTROL_MASK ) {
                        g3dui_pasteSelection ( gui );
                    }
                } break;

                case GDK_KEY_s: {
                    if ( kev->state & GDK_CONTROL_MASK ) {
                        gtk3_saveFile ( gtk3gui );
                    }
                } break;

                case GDK_KEY_a: {
                    if ( kev->state & GDK_CONTROL_MASK ) {
                        g3dui_selectAll ( gui );
                    }
                } break;
            }
        } break;

        case GDK_BUTTON_PRESS : {
            G3DUIRENDERPROCESS *rps = g3dui_getRenderProcessByID ( gui, ( uint64_t ) widget );
            /*** If there was a running render, cancel it and dont go further ***/
            if ( rps ) {
                q3djob_end ( rps->qjob );

                pthread_join ( rps->tid, NULL );

                return TRUE;
            }

            /*** For keyboard inputs ***/
            gtk_widget_grab_focus ( widget );
        } break;

        default :
        break;
    }

    if ( gui->curmou ) {
        /*** Call the mousetool callback and redraw if return value is > 0 ***/
        G3DEvent g3dev;

        gdkevent_to_g3devent ( gdkev, &g3dev );

        if ( gui->curmou->tool->event ) {
            uint32_t msk = gui->curmou->tool->event ( gui->curmou->tool, 
                                                      gui->sce,
                                                      view->cam, 
                                                      gui->urm,
                                                      gui->engine_flags,
                                                     &g3dev );

            gtk3_interpretUIReturnFlags ( gtk3gui, msk );

            if ( gdkev->type == GDK_BUTTON_RELEASE ) {
                g3dcursor_reset ( &gui->sce->csr );
            }
        }
    }

    return TRUE;
}

/******************************************************************************/
static void sizeGL ( GtkWidget     *widget,
                     GtkAllocation *allocation, 
                     gpointer       user_data ) {
    GTK3G3DUIVIEW *gtk3view =  ( GTK3G3DUIVIEW * ) user_data;
    G3DUIVIEW     *view    = &gtk3view->core;
    G3DUI         *gui     = view->gui;
    /*GdkDisplay    *gdkdpy  =  gtk_widget_get_display ( widget );
    GdkWindow     *gdkwin  =  gtk_widget_get_window ( widget );*/

/*printf("glarea %d %d\n", allocation->width, allocation->height );*/

    /*if ( gdkwin ) {*/
#ifdef TODO : find why I get allocation.{width,height} = 1
#endif
        if ( (allocation->width > 1 ) && (allocation->height > 1) ) {
            /*** cancel renderprocess if any ***/
            g3dui_cancelRenderByID ( gui, ( uint64_t ) widget );

            g3duiview_sizeGL ( view, allocation->width, allocation->height );

            /*gtk_widget_queue_draw ( widget );*/
        }
   /* }*/
}

/******************************************************************************/
static void initGL ( GtkWidget *widget,
                     gpointer   user_data ) {
    GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) user_data;
    G3DUIVIEW     *view    = &gtk3view->core;

}

/******************************************************************************/
static gboolean showGL ( GtkWidget *widget, 
                         cairo_t   *cr, 
                         gpointer   user_data ) {
    GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) user_data;
    G3DUIVIEW     *view    = &gtk3view->core;

    g3duiview_showGL ( view, view->engine_flags );

    return FALSE;
}

/******************************************************************************/
static void mapGL ( GtkWidget *widget, 
                    gpointer   user_data ) {
    GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) user_data;
    G3DUIVIEW     *view    = &gtk3view->core;
    GdkDisplay    *gdkdpy  = gtk_widget_get_display ( widget );
#ifdef __linux__
    GdkWindow     *gdkwin  = gtk_widget_get_window ( widget );
    Display       *dpy     = gdk_x11_display_get_xdisplay ( gdkdpy );
    Window         win     = gdk_x11_window_get_xid ( gdkwin );

    gtk3view->core.dpy = dpy;
    gtk3view->core.win = win;
#endif
#ifdef __MINGW32__
    HWND hWnd = GDK_WINDOW_HWND ( gdkwin );

    gtk3view->core.hWnd = hWnd;
#endif

    g3duiview_initGL ( view );
}

/******************************************************************************/
static GTK3G3DUIVIEW *gtk3_g3duiview_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memSize =  sizeof ( GTK3G3DUIVIEW );
    GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) calloc ( 0x01, memSize );

    if ( gtk3view == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;

    }

    gtk3view->core.gui = ( G3DUI * ) gtk3gui;
    gtk3view->core.pressedButtonID = -1;


    return gtk3view;
}

/******************************************************************************/
/*static void SizeAllocate ( GtkWidget     *widget,
                           GtkAllocation *allocation,
                           gpointer       user_data ) {*/

static gboolean SizeAllocate ( GtkWidget        *self,
                               GdkEventConfigure event,
                               gpointer          user_data ) {
    GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) user_data;
    GTK3G3DUIMENU *gtk3menu = ( GTK3G3DUIMENU * ) gtk3view->core.menuBar;
    GdkRectangle gdkrec;

    /*g3duiview_resize ( &gtk3view->core,
                        event.width,
                        event.height );*/

#ifdef unused
    /*** Menu ***/
    if ( gtk3menu->menu ) {
        g3duirectangle_toGdkRectangle ( &gtk3view->core.menurec, &gdkrec );

        gtk_widget_size_allocate ( gtk3menu->menu, &gdkrec );
    }

    /*** GL Area ***/
    if ( gtk3view->glarea ) {
        g3duirectangle_toGdkRectangle ( &gtk3view->core.glrec, &gdkrec );

        gtk_widget_size_allocate ( gtk3view->glarea, &gdkrec );
    }

    /*** Navigation Bar ***/
    if ( gtk3view->navbar ) {
        g3duirectangle_toGdkRectangle ( &gtk3view->core.navrec, &gdkrec );

        gtk_widget_size_allocate ( gtk3view->navbar, &gdkrec );
    }
#endif
    return FALSE;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) user_data;

    free ( gtk3view );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) user_data;


}

/******************************************************************************/
void gtk3_g3duiview_updateMenuBar ( GTK3G3DUIVIEW *gtk3view ) {
    gtk3_g3duimenu_update_r ( ( GTK3G3DUIMENU * ) gtk3view->core.menuBar );
}

/******************************************************************************/
static void gtk3_g3duiview_createMenuBar ( GTK3G3DUIVIEW *gtk3view ) {
    GTK3G3DUIMENU *gtk3menu;

    gtk3menu = gtk3_g3duimenu_parse_r ( g3duimenu_getViewMenuNode ( ),
                                        gtk3view->core.gui,
                                        gtk3view );

    gtk3_g3duimenu_update_r ( gtk3menu );

    gtk_layout_put ( GTK_LAYOUT(gtk3view->layout), gtk3menu->menu, 0, 0 );

    gtk_widget_show ( gtk3menu->menu );


    gtk3view->core.menuBar = ( G3DUIMENU * ) gtk3menu;
}

/******************************************************************************/
static void gtk3_g3duiview_createGLArea ( GTK3G3DUIVIEW *gtk3view ) {
    G3DUI *gui = gtk3view->core.gui;
    GtkWidget    *glarea = gtk_drawing_area_new ( );
GdkVisual* visual;
GdkScreen *screen;

#ifdef __linux__
XVisualInfo *xvisual;
Colormap xcolormap;
Display *display;
int xscreen;
Window root;

    /*** the OpenGL Window ***/
    gtk_widget_set_double_buffered ( glarea, FALSE );

int attributes[] = { GLX_RGBA, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1,
GLX_BLUE_SIZE, 1, GLX_DOUBLEBUFFER, True, GLX_DEPTH_SIZE, 12, None };
display = gdk_x11_get_default_xdisplay ();
xscreen = DefaultScreen (display);
screen = gdk_screen_get_default ();
xvisual = glXChooseVisual (display, xscreen, attributes);
visual = gdk_x11_screen_lookup_visual (screen, xvisual->visualid);
root = RootWindow (display, xscreen);
xcolormap = XCreateColormap (display, root, xvisual->visual, AllocNone);
gtk_widget_set_visual(glarea, visual);


    gtk3view->core.glctx = glXCreateContext ( display, 
                                              xvisual, 
                                              gui->sharedCtx,
                                              TRUE);

    /*** share textures ***/
    if ( gui->sharedCtx == NULL ) gui->sharedCtx = gtk3view->core.glctx;

    free (xvisual);
#endif /* __linux__ */

    /*** For keyboard inputs ***/
    gtk_widget_set_can_focus ( glarea, TRUE );

    /*** Drawing area does not receive mous events by defaults ***/
    gtk_widget_set_events ( glarea, 
                            gtk_widget_get_events ( glarea ) |
                            GDK_KEY_PRESS_MASK                         |
			                GDK_KEY_RELEASE_MASK                       |
                            GDK_BUTTON_PRESS_MASK                      |
                            GDK_BUTTON_RELEASE_MASK                    |
                            GDK_POINTER_MOTION_MASK                    |
                            GDK_POINTER_MOTION_HINT_MASK );

    g_signal_connect ( G_OBJECT (glarea), "size-allocate"     , G_CALLBACK (sizeGL ), gtk3view );
    g_signal_connect ( G_OBJECT (glarea), "realize"           , G_CALLBACK (initGL ), gtk3view );
    g_signal_connect ( G_OBJECT (glarea), "draw"              , G_CALLBACK (showGL ), gtk3view );
    g_signal_connect ( G_OBJECT (glarea), "map"               , G_CALLBACK (mapGL  ), gtk3view );

    g_signal_connect ( G_OBJECT (glarea), "motion_notify_event" , G_CALLBACK (inputGL), gtk3view );
    g_signal_connect ( G_OBJECT (glarea), "key_press_event"     , G_CALLBACK (inputGL), gtk3view );
    g_signal_connect ( G_OBJECT (glarea), "key_release_event"   , G_CALLBACK (inputGL), gtk3view );
    g_signal_connect ( G_OBJECT (glarea), "button_press_event"  , G_CALLBACK (inputGL), gtk3view );
    g_signal_connect ( G_OBJECT (glarea), "button_release_event", G_CALLBACK (inputGL), gtk3view );


    gtk_layout_put ( GTK_LAYOUT(gtk3view->layout), glarea, 0, 0 );

    gtk_widget_show ( glarea );

    gtk3view->glarea = glarea;
}

/******************************************************************************/
static void redrawGLArea ( GTK3G3DUIVIEW  *gtk3view ) {
    GdkWindow *window = gtk_widget_get_window ( gtk3view->glarea );
    /*GdkRectangle arec;

    arec.x = arec.y = 0x00;
    arec.width = arec.height = 1;*/

    gdk_window_invalidate_rect ( window, NULL, FALSE );
}

/******************************************************************************/
static void grabPointer ( GtkWidget *widget,
                          GdkEvent  *event ) {
    gdk_device_grab ( gdk_event_get_device  ( event  ),
                      gtk_widget_get_window ( widget ),
                      GDK_OWNERSHIP_WINDOW,
                      FALSE, 
                      GDK_POINTER_MOTION_MASK |
                      GDK_BUTTON_PRESS_MASK   | 
                      GDK_BUTTON_RELEASE_MASK,
                      NULL,
                      GDK_CURRENT_TIME );
}

/******************************************************************************/
static void ungrabPointer ( GtkWidget *widget,
                            GdkEvent  *event ) {
    gdk_device_ungrab ( gdk_event_get_device  ( event  ), GDK_CURRENT_TIME );
}

/******************************************************************************/
static void movePointer ( GtkWidget *widget, 
                          GdkEvent  *event,
                          uint32_t   x,
                          uint32_t   y ) {
    GtkWidget *top = gtk_widget_get_toplevel ( widget );
    gint winx, winy;

    /*** gdk_device_warp() puts the pointer into screen coordinates. ***/
    /*** We have to translate the widgets coordinates to screen ones ***/
    gdk_window_get_origin ( gtk_widget_get_window ( widget ), &winx, &winy );

    gdk_device_warp ( gdk_event_get_device  ( event  ), 
                      gtk_widget_get_screen ( widget ), winx + x, winy + y );

    /*printf ( "reseting mouse to %d %d\n", winx + x, winy + y );*/
}

/******************************************************************************/
static void redrawNavigationBar ( GtkWidget *widget ) {
    GdkWindow *window = gtk_widget_get_window ( widget );
    uint32_t width  = gtk_widget_get_allocated_width  ( widget ),
             height = gtk_widget_get_allocated_height ( widget );
    GdkRectangle arec;

    arec.x = arec.y = 0x00;
    arec.width  = width;
    arec.height = height;

    gdk_window_invalidate_rect ( window, &arec, FALSE );
}

/******************************************************************************/
static void motionNotify ( GTK3G3DUIVIEW  *gtk3view,
                           GdkModifierType state,
                           G3DPIVOT       *piv,
                           float           difx,
                           float           dify ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) gtk3view;

    if ( ( state & GDK_CONTROL_MASK ) &&
         ( state & GDK_SHIFT_MASK   ) ) {
            difx *= 100;
            dify *= 100;
    } else {
        if ( state & GDK_CONTROL_MASK ) {
            difx /= 10;
            dify /= 10;
        }

        if ( state & GDK_SHIFT_MASK ) {
            difx *= 10;
            dify *= 10;
        }
    }

    switch ( view->pressedButtonID ) {
        case ROTATEBUTTON : {
            if ( state & GDK_BUTTON1_MASK ) {
                g3duiview_orbit ( view, 
                                  piv,
                                  difx, 
                                  dify );
            }

            if ( state & GDK_BUTTON3_MASK ) {
                g3duiview_spin ( view, 
                                 difx );
            }
        } break;

        case ZOOMBUTTON : {
            if ( state & GDK_BUTTON3_MASK ) {
                g3duiview_zoom ( view, -difx );
            }

            if ( state & GDK_BUTTON1_MASK ) {
                g3duiview_moveForward ( view, difx );
            }

            if ( view->cam != view->defcam ) {
                /*** update camera focal value ***/
                gtk3_interpretUIReturnFlags ( gtk3view->core.gui, UPDATECURRENTOBJECT ); 
            }
        } break;

        case TRANSLATEBUTTON : {
            if ( state & GDK_BUTTON1_MASK ) {
                g3duiview_moveSideward ( view,
                                         difx,
                                         dify );
            }

            if ( state & GDK_BUTTON3_MASK ) {
                g3duiview_moveForward ( view, difx );
            }
        } break;

        default:
        break;
    }
}

/******************************************************************************/
static gboolean navInput ( GtkWidget *widget,
                           GdkEvent  *event,
                           gpointer user_data ) {
    GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) user_data;
    G3DUIVIEW *view = &gtk3view->core;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3view->core.gui;
    G3DUI *gui = &gtk3gui->core;
    static G3DPIVOT *piv;
    G3DCAMERA *cam = view->cam;
    G3DSCENE *sce = gui->sce;
    static uint32_t grabbing;
    static int xori, yori, xold, yold;

    switch ( event->type ) {
        case GDK_BUTTON_PRESS : {
            GdkEventButton *bev = ( GdkEventButton * ) event;

            g3dui_cancelRenderByID ( gui, ( uint64_t ) gtk3view->glarea );

            g3duiview_pressButton ( view, bev->x, bev->y );

            xori = xold = bev->x;
            yori = yold = bev->y;

            if ( view->pressedButtonID != MAXIMIZEBUTTON ) {
                GdkWindow *gdkwin = gtk_widget_get_window ( gtk3gui->topWin );

                /*** Hide the mouse cursor ***/
                gdk_window_set_cursor ( gdkwin, 
                                        gdk_cursor_new ( GDK_BLANK_CURSOR ) );

                switch ( view->pressedButtonID ) {
                    case ROTATEBUTTON : {
                        /*** pivot for rotation ***/
                        piv = g3dpivot_new ( cam, 
                                            &sce->csr,
                                             gui->engine_flags );
                    } break;
                }

                grabPointer ( widget, event );

                grabbing = 0x01;
            }

            redrawNavigationBar ( widget );
        } break;

        case GDK_MOTION_NOTIFY : {
            GdkEventMotion *mev = ( GdkEventMotion * ) event;

            if ( grabbing ) {
                float difx = ( xold - mev->x ) * 0.5f,
                      dify = ( yold - mev->y ) * 0.5f;

                motionNotify ( gtk3view,
                               mev->state,
                               piv,
                               difx,
                               dify );

                redrawGLArea ( gtk3view );

                if ( ( xori != mev->x ) || ( yori != mev->y ) ) {
                    /*** this must be called before the gtk_events_pending()***/
                    /*** thing because it's also dispatched by the event loop**/
                    /*** and we absolutely want the event to be generated ***/
                    /*** right after the redrawing of the gl area ***/
                    movePointer ( widget, mev, xori, yori );
                }
            }

            xold = xori;
            yold = yori;
        } break;

        case GDK_BUTTON_RELEASE : {
            GdkEventButton *bev = ( GdkEventButton * ) event;

            if ( view->pressedButtonID != MAXIMIZEBUTTON ) {
                GdkWindow *gdkwin = gtk_widget_get_window ( gtk3gui->topWin );

                gdk_window_set_cursor ( gdkwin, NULL );
            } else {
                if ( gui->main->quad ) {
                    GTK3G3DUIQUAD *gtk3quad = ( GTK3G3DUIQUAD * ) gui->main->quad;

                    gtk3_g3duiquad_rearrange ( gtk3quad, gtk3view );
                }
            }

            g3duiview_releaseButton ( view );

            if ( grabbing ) ungrabPointer ( widget, event );

            grabbing = 0x00;

            redrawNavigationBar ( widget );

            if ( piv ) {
                g3dobject_free ( ( G3DOBJECT * ) piv );

                piv = NULL;
            }
        } break;

        default:
        break;
    }

    return TRUE;
}

/******************************************************************************/
static gboolean navDraw ( GtkWidget *widget, 
                          cairo_t   *cr, 
                          gpointer   user_data ) {
    GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) user_data;
    G3DUIVIEW *view = &gtk3view->core;
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    uint32_t width  = gtk_widget_get_allocated_width  ( widget ),
             height = gtk_widget_get_allocated_height ( widget );
    uint32_t i;

    gtk_render_background ( context, cr, 0x00, 0x00, width, height );

    for ( i = 0x00; i < NBVIEWBUTTON; i++ ) {
        GdkPixbuf *icon = gtk3view->pix[i];

        if ( i == view->pressedButtonID ) {
            cairo_set_source_rgb ( cr, 0.25f, 0.25f, 0.25f );

            cairo_rectangle ( cr, view->pixrec[i].x, 
                                  view->pixrec[i].y, 
                                  view->pixrec[i].width, 
                                  view->pixrec[i].height );
            cairo_fill ( cr );
        }

        gdk_cairo_set_source_pixbuf ( cr, icon, view->pixrec[i].x, 
                                                view->pixrec[i].y );
        cairo_paint ( cr );
    }

    return FALSE;
}

/******************************************************************************/
static void gtk3_g3duiview_createNavigationBar ( GTK3G3DUIVIEW *gtk3view ) {
    GtkWidget *navbar = ui_gtk_drawing_area_new ( CLASS_VIEW_MENU_BAR );
    GtkStyleContext *context = gtk_widget_get_style_context ( navbar );

    gtk_style_context_add_class ( context, CLASS_MAIN );

    gtk3view->navbar = navbar;

    /*** Drawing area does not receive mous events by defaults ***/
    gtk_widget_set_events ( navbar, 
                            gtk_widget_get_events ( navbar )  |
                            GDK_KEY_PRESS_MASK                |
			                GDK_KEY_RELEASE_MASK              |
                            GDK_BUTTON_PRESS_MASK             |
                            GDK_BUTTON_RELEASE_MASK           |
                            GDK_POINTER_MOTION_MASK           |
                            GDK_POINTER_MOTION_HINT_MASK );

    g_signal_connect ( G_OBJECT (navbar), "draw"                , G_CALLBACK (navDraw ), gtk3view );
    g_signal_connect ( G_OBJECT (navbar), "motion_notify_event" , G_CALLBACK (navInput), gtk3view );
    g_signal_connect ( G_OBJECT (navbar), "key_press_event"     , G_CALLBACK (navInput), gtk3view );
    g_signal_connect ( G_OBJECT (navbar), "key_release_event"   , G_CALLBACK (navInput), gtk3view );
    g_signal_connect ( G_OBJECT (navbar), "button_press_event"  , G_CALLBACK (navInput), gtk3view );
    g_signal_connect ( G_OBJECT (navbar), "button_release_event", G_CALLBACK (navInput), gtk3view );



    gtk3view->pix[MAXIMIZEBUTTON]  = gdk_pixbuf_new_from_xpm_data ( maximize_view_xpm  );
    gtk3view->pix[ROTATEBUTTON]    = gdk_pixbuf_new_from_xpm_data ( rotate_view_xpm    );
    gtk3view->pix[TRANSLATEBUTTON] = gdk_pixbuf_new_from_xpm_data ( translate_view_xpm );
    gtk3view->pix[ZOOMBUTTON]      = gdk_pixbuf_new_from_xpm_data ( zoom_view_xpm      );

    gtk_layout_put ( GTK_LAYOUT(gtk3view->layout), navbar, 0, 0 );

    gtk_widget_show ( navbar );
}

/******************************************************************************/
void gtk3_g3duiview_resize ( GTK3G3DUIVIEW *gtk3view,
                             uint32_t       width,
                             uint32_t       height ) {
    GTK3G3DUIMENU *menuBar = ( GTK3G3DUIMENU * ) gtk3view->core.menuBar;
    GdkRectangle gdkrec;

/*printf("%d\n", gtk_widget_get_allocated_height ( menuBar->menu ) );*/

    g3duiview_resize ( &gtk3view->core,
                        width,
                        height,
                        BUTTONSIZE );

    /*** Menu ***/
    if ( gtk3view->core.menuBar ) {
        GTK3G3DUIMENU *gtk3menu = menuBar;

        g3duirectangle_toGdkRectangle ( &gtk3view->core.menurec, &gdkrec );

        gtk_layout_move ( gtk3view->layout,
                         gtk3menu->menu, 
                         gdkrec.x,
                         gdkrec.y );

        gtk_widget_set_size_request ( gtk3menu->menu,
                                      gdkrec.width,
                                      gdkrec.height );
    }

    /*** GL Area ***/
    if ( gtk3view->glarea ) {
        g3duirectangle_toGdkRectangle ( &gtk3view->core.glrec, &gdkrec );

        gtk_layout_move ( gtk3view->layout,
                         gtk3view->glarea, 
                         gdkrec.x,
                         gdkrec.y );

        gtk_widget_set_size_request ( gtk3view->glarea,
                                      gdkrec.width,
                                      gdkrec.height );
    }

    /*** Navigation Bar ***/
    if ( gtk3view->navbar ) {
        g3duirectangle_toGdkRectangle ( &gtk3view->core.navrec, &gdkrec );

        gtk_layout_move ( gtk3view->layout,
                          gtk3view->navbar, 
                          gdkrec.x,
                          gdkrec.y );

        gtk_widget_set_size_request ( gtk3view->navbar,
                                      gdkrec.width,
                                      gdkrec.height );
    }
}

/******************************************************************************/
GTK3G3DUIVIEW *gtk3_g3duiview_create ( GtkWidget *parent,
                                       GTK3G3DUI *gtk3gui,
                                       char      *name ) {
    GTK3G3DUIVIEW *gtk3view = gtk3_g3duiview_new ( gtk3gui );
    GtkWidget    *layout = ui_gtk_layout_new ( CLASS_MAIN, NULL, NULL );

    gtk3view->layout = layout;

    gtk_widget_set_name ( layout, name );

    /*gtk_widget_add_events(GTK_WIDGET(layout), GDK_CONFIGURE);*/

    g_signal_connect ( G_OBJECT (layout), "realize", G_CALLBACK (Realize), gtk3view );
    g_signal_connect ( G_OBJECT (layout), "destroy", G_CALLBACK (Destroy), gtk3view );

    gtk_widget_show ( layout );

    gtk3_g3duiview_createGLArea ( gtk3view );
    gtk3_g3duiview_createMenuBar ( gtk3view );
    gtk3_g3duiview_createNavigationBar ( gtk3view );


    return gtk3view;
}

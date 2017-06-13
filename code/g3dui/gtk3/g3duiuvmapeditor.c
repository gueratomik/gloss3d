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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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

G_DEFINE_TYPE (GtkUVMapEditor, gtk_uvmapeditor, GTK_TYPE_FIXED)

/******************************************************************************/
static void     gtk_uvmapeditor_class_init    ( GtkUVMapEditorClass * );
static void     gtk_uvmapeditor_init          ( GtkUVMapEditor * );
static void     gtk_uvmapeditor_realize       ( GtkWidget * );
static void     gtk_uvmapeditor_size_request  ( GtkWidget *, GtkRequisition * );
static gboolean gtk_uvmapeditor_expose        ( GtkWidget *, cairo_t * );
static gboolean gtk_uvmapeditor_configure     ( GtkWidget *, GdkEvent *, gpointer );
static void     gtk_uvmapeditor_size_allocate ( GtkWidget *, GtkAllocation * );
static void     gtk_uvmapeditor_event         ( GtkWidget *, GdkEvent *, gpointer );

gboolean g3duiuvmapeditor_showGL ( GtkWidget *widget, 
                                   cairo_t   *cr,
                                   gpointer   user_data );
void g3duiuvmapeditor_initGL ( GtkWidget *widget, 
                               gpointer   user_data );
void g3duiuvmapeditor_sizeGL ( GtkWidget    *widget, 
                               GdkRectangle *allocation, 
                               gpointer      user_data );
void g3duiuvmapeditor_inputGL ( GtkWidget *widget,
                                GdkEvent  *gdkev, 
                                gpointer   user_data );

/******************************************************************************/
GtkWidget *createUVMapMenu ( GtkWidget *widget, G3DUI *gui, char *menuname ) {
    GtkUVMapEditor *guv = ( GtkUVMapEditor * ) widget;
    GtkWidget *menu;

    menu = gtk_menu_new ( );

    gtk_widget_set_name ( menu, menuname );

    gtk_widget_show ( menu );


    return menu;
}

/******************************************************************************/
GtkWidget *gtk_uvmapeditor_getDrawingArea ( GtkWidget *widget ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;

        if ( GTK_IS_DRAWING_AREA(child) ) {

            return child;
        }

        children = g_list_next ( children );
    }


    return NULL;
}

/******************************************************************************/
static void gtk_uvmapeditor_class_init ( GtkUVMapEditorClass *uvme_class ) {

    GObjectClass   *object_class = G_OBJECT_CLASS   ( uvme_class );
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS ( uvme_class );
    GdkPixbuf     **icon         = uvme_class->icon;

    icon[UVMAPTRANSLATEBUTTON] = gdk_pixbuf_new_from_xpm_data ( translate_view_xpm );
    icon[UVMAPZOOMBUTTON]      = gdk_pixbuf_new_from_xpm_data ( zoom_view_xpm      );

  /*object_class->set_property  = pfx_foo_set_property;
  object_class->get_property  = pfx_foo_get_property;*/

    widget_class->realize          = gtk_uvmapeditor_realize;
    /*widget_class->configure_event  = gtk_uvmapeditor_configure;*/
    widget_class->size_allocate    = gtk_uvmapeditor_size_allocate;
    /*widget_class->adjust_size_request   = gtk_uvmapeditor_adjust_size_request;*/
    widget_class->draw             = gtk_uvmapeditor_expose;
/*** this is now set in gtk_uvmapeditor_new () so I can set GUI as user_data ***/
    /*widget_class->event            = gtk_uvmapeditor_event;*/

  /* ajout de la propri\E9t\E9 PfxFoo:active */
  /*g_object_class_install_property (object_class, PROP_ACTIVE,
                                   g_param_spec_boolean ("active",
                                                         "Active",
                                                         "Whether the display is active",
                                                         FALSE,
                                                         G_PARAM_READWRITE));*/
   
  /* ajout d'un \E9l\E9ment priv\E9 (utilis\E9 pour contenit les champs priv\E9s de
   * l'objet) */
  /*g_type_class_add_private (klass, sizeof (PfxFooPrivate));*/
}

/******************************************************************************/
static void gtk_uvmapeditor_move_pointer ( GtkWidget *widget, GdkEvent *event,
                                                       uint32_t x,
                                                       uint32_t y ) {
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
static void gtk_uvmapeditor_grab_pointer ( GtkWidget *widget, GdkEvent *event ) {
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
static void gtk_uvmapeditor_ungrab_pointer ( GtkWidget *widget, GdkEvent *event ) {
    gdk_device_ungrab ( gdk_event_get_device  ( event  ), GDK_CURRENT_TIME );
}

/******************************************************************************/
static void gtk_uvmapeditor_redraw_area ( GtkWidget *widget ) {
    GtkWidget *area = gtk_uvmapeditor_getDrawingArea ( widget );
    GdkRectangle arec;

    /*** Tell cairo to shut the f*** up ***/
    arec.x = arec.y = 0;
    arec.width = arec.height = 1;

    gdk_window_invalidate_rect ( gtk_widget_get_window ( area ), &arec, FALSE );
}

/******************************************************************************/
static void gtk_uvmapeditor_redraw_menu ( GtkWidget *widget ) {
    uint32_t width = gtk_widget_get_allocated_width  ( widget );

    cairo_rectangle_int_t crec = { 0, 0, width, BUTTONSIZE };
    cairo_region_t *creg = cairo_region_create_rectangle ( &crec );

    /*** if we call gtk_widget_queue_draw, GL area is going to ***/
    /*** flicker because GDK erases the whole background and it ***/
    /*** seems there is no way to disable this behavior. Thus, ***/
    /*** we just erase the menubar region ***/
    gdk_window_invalidate_region ( gtk_widget_get_window ( widget ),
                                   creg, 
                                   FALSE );

    cairo_region_destroy ( creg );
}

/******************************************************************************/
static void gtk_uvmapeditor_event ( GtkWidget *widget, GdkEvent *event,
                                                gpointer user_data ) {
    static int xmid, ymid, xori, yori;
    GtkUVMapEditor *guv = ( GtkUVMapEditor * ) widget;
    G3DUIUVMAPEDITOR *uvme = &guv->uvme;
    static int xold, yold, grabbing;
    uint32_t width, height;
    static G3DPIVOT *piv;
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt  = ( G3DUIGTK3 * ) gui->toolkit_data;
    G3DSCENE *sce = gui->sce;
    static GtkWidget *area;

    width  = gtk_widget_get_allocated_width  ( widget );
    height = gtk_widget_get_allocated_height ( widget );

    switch ( event->type ) {
        case GDK_BUTTON_PRESS : {
            GdkEventButton *bev = ( GdkEventButton * ) event;
            area = gtk_uvmapeditor_getDrawingArea ( widget );

            /*** cancel renderprocess if any ***/
            /*g3dui_cancelRenderByID ( gui, ( uint64_t ) area );*/

            G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, ( uint64_t ) area );
            /*** If there was a running render, cancel it and dont go further ***/
            if ( rps ) {
                r3dscene_cancelRender ( rps->rsce );
            }

            uvme->buttonID = common_g3duiuvmapeditor_getCurrentButton ( uvme, bev->x, 
                                                                              bev->y );

            xori = xold = bev->x;
            yori = yold = bev->y;

            if ( grabbing == 0x00 ) { 
                gdk_window_set_cursor ( gtk_widget_get_window ( ggt->top ), 
                                        gdk_cursor_new ( GDK_BLANK_CURSOR ) );

                gtk_uvmapeditor_grab_pointer ( widget, event );

                grabbing = 0x01;
            }

            gtk_uvmapeditor_redraw_menu ( widget );
        } break;

        case GDK_BUTTON_RELEASE : {
            GdkEventButton *bev = ( GdkEventButton * ) event;

            if ( uvme->buttonID > -1 ) {
                switch ( uvme->buttonID ) {
                    default:
                        if ( grabbing ) {
                            gtk_uvmapeditor_ungrab_pointer ( widget, event );

                            gdk_window_set_cursor ( gtk_widget_get_window ( ggt->top ), 
                                                NULL );

                            grabbing = 0x00;
                        }
                    break;
                }
            }

            /*** Reset selected button flag ***/
            uvme->buttonID = -1;

            gtk_uvmapeditor_redraw_menu ( widget );
        } break;

        case GDK_MOTION_NOTIFY : {
            GdkEventMotion *mev = ( GdkEventMotion * ) event;

            /*** If pressed ***/
            if ( uvme->buttonID > -1 && grabbing ) {

                switch ( uvme->buttonID ) {
                    case UVMAPZOOMBUTTON : {
                        if ( mev->state & GDK_BUTTON1_MASK ) {
                            common_g3duiuvmapeditor_moveForward ( uvme, 
                                                                  mev->x, 
                                                                  xold );
                        }
                    } break;

                    case UVMAPTRANSLATEBUTTON : {
                        if ( mev->state & GDK_BUTTON1_MASK ) {
                            common_g3duiuvmapeditor_moveSideward ( uvme,
                                                                   mev->x,
                                                                   mev->y, 
                                                                   xold, 
                                                                   yold );
                        }

                        if ( mev->state & GDK_BUTTON3_MASK ) {
                            common_g3duiuvmapeditor_moveForward ( uvme, 
                                                                  mev->x, 
                                                                  xold );
                        }
                    } break;

                    default:
                    break;
                }

                gtk_uvmapeditor_redraw_area ( widget );

                if ( ( xori != mev->x ) || ( yori != mev->y ) ) {
                    /*** this must be called before the gtk_events_pending()***/
                    /*** thing because it's also dispatched by the event loop**/
                    /*** and we absolutely want the event to be generated ***/
                    /*** right after the redrawing of the gl area ***/
                    gtk_uvmapeditor_move_pointer ( widget, event, xori, yori );
                }

                /*** while (gtk_events_pending ()) gtk_main_iteration ();***/
/*while (gtk_events_pending ()) gtk_main_iteration ( );*/
            }

            xold = xori;
            yold = yori;
        } break;

        default:
        break;
    }
}

/******************************************************************************/
static void gtk_uvmapeditor_size_allocate ( GtkWidget     *widget,
                                     GtkAllocation *allocation ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    GtkUVMapEditor *guv = ( GtkUVMapEditor * ) widget;

    common_g3duiuvmapeditor_resize ( &guv->uvme, 
                                      allocation->width, 
                                      allocation->height );

    gtk_widget_set_allocation ( widget, allocation );

    if ( gtk_widget_get_realized ( widget ) ) {
        gdk_window_move_resize ( gtk_widget_get_window ( widget ),
                                 allocation->x,
                                 allocation->y,
                                 allocation->width, 
                                 allocation->height );
    }

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );
        GdkRectangle gdkrec = { 0x00, 0x00, 0x00, 0x00 };

        if ( gtk_widget_get_has_window ( widget ) == 0x00 ) {
            gdkrec.x += allocation->x;
            gdkrec.y += allocation->y;
        }

        if ( strcmp ( child_name, "MODEBAR" ) == 0x00 ) {
            gdkrec.x      += 0;
            gdkrec.y      += 0;
            gdkrec.width   = MODEBARBUTTONSIZE;
            gdkrec.height  = allocation->height;

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        if ( GTK_IS_DRAWING_AREA(child) ) {
            gdkrec.x      += MODEBARBUTTONSIZE;
            gdkrec.y      += BUTTONSIZE;
            gdkrec.width   = allocation->width - MODEBARBUTTONSIZE;
            gdkrec.height  = allocation->height - BUTTONSIZE;

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        children = g_list_next ( children );
    }
}

/******************************************************************************/
static gboolean gtk_area_filter ( GdkXEvent *xevent,
                  GdkEvent *event,
                  gpointer data ) {
    #ifdef __MINGW32__
    MSG *msg = ( MSG * ) xevent;
    /*** use parent class configure function ***/
    /*GTK_WIDGET_CLASS(gtk_uvmapeditor_parent_class)->configure_event ( widget, event );*/

    if ( msg->message == 70 ) return GDK_FILTER_REMOVE;

    #endif

    return GDK_FILTER_CONTINUE;
}

/******************************************************************************/
static gboolean gtk_uvmapeditor_expose ( GtkWidget *widget, cairo_t *cr ) {
    GtkUVMapEditorClass *guv_class = ( GtkUVMapEditorClass * ) G_OBJECT_GET_CLASS ( widget);
    GtkUVMapEditor *guv = ( GtkUVMapEditor * ) widget;
    G3DUIUVMAPEDITOR *uvme = &guv->uvme;

    if ( gtk_widget_is_drawable ( widget ) ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( widget );
        uint32_t width  = gtk_widget_get_allocated_width  ( widget ),
                 height = gtk_widget_get_allocated_height ( widget );
        GtkAllocation alloc;
        uint32_t i;

        gtk_render_background ( context, cr, 0x00, 0x00, width, height );

        for ( i = 0x00; i < NBUVMAPBUTTON; i++ ) {
            GdkPixbuf *icon = guv_class->icon[i];

            if ( i == uvme->buttonID ) {
                cairo_set_source_rgb ( cr, 0.25f, 0.25f, 0.25f );

                cairo_rectangle ( cr, uvme->rec[i].x, 
                                      uvme->rec[i].y, 
                                      uvme->rec[i].width, 
                                      uvme->rec[i].height );
                cairo_fill ( cr );
            }

            gdk_cairo_set_source_pixbuf ( cr, icon, uvme->rec[i].x, 
                                                    uvme->rec[i].y );
            cairo_paint ( cr );
        }
    }
 
    return GTK_WIDGET_CLASS (gtk_uvmapeditor_parent_class)->draw (widget, cr);
}

/******************************************************************************/
static void gtk_uvmapeditor_init ( GtkUVMapEditor *guv ) {
    GtkWidget *widget = ( GtkWidget * ) guv;
    G3DUIUVMAPEDITOR *uvme   = &guv->uvme;

    /*** Expose event won't be called if we dont set has_window ***/
    gtk_widget_set_has_window ( widget, TRUE );

    common_g3duiview_init ( uvme, gtk_widget_get_allocated_width  ( widget ), 
                                  gtk_widget_get_allocated_height ( widget ) );
}

/******************************************************************************/
static void gtk_uvmapeditor_realize ( GtkWidget *widget ) {
    GdkWindow *parent_window = gtk_widget_get_parent_window ( widget );
    GtkStyleContext  *style  = gtk_widget_get_style_context ( widget );
    GtkUVMapEditor *guv = ( GtkUVMapEditor * ) widget;
    G3DUIUVMAPEDITOR *uvme = &guv->uvme;
    GdkWindowAttr attributes;
    GtkAllocation allocation;
    GdkWindow *window;

    gtk_widget_set_realized ( widget, TRUE );

    /******************/

    gtk_widget_get_allocation ( widget, &allocation );

    attributes.window_type  = GDK_WINDOW_CHILD;
    attributes.visual       = gtk_widget_get_visual ( widget );
    attributes.x            = allocation.x;
    attributes.y            = allocation.y;
    attributes.width        = allocation.width;
    attributes.height       = allocation.height;
    attributes.wclass       = GDK_INPUT_OUTPUT;
    attributes.event_mask   = gtk_widget_get_events (widget);

    common_g3duiuvmapeditor_init ( uvme, allocation.width, allocation.height );

    attributes.event_mask  |=  ( GDK_EXPOSURE_MASK            |
                                 GDK_KEY_PRESS_MASK           |
			                     GDK_KEY_RELEASE_MASK         |
                                 GDK_BUTTON_PRESS_MASK        |
                                 GDK_BUTTON_RELEASE_MASK      |
                                 GDK_POINTER_MOTION_MASK      |
                                 GDK_POINTER_MOTION_HINT_MASK |
			                     GDK_STRUCTURE_MASK );

    window = gdk_window_new ( parent_window, &attributes, GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL );

    gtk_widget_set_window   ( widget, window );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_widget_register_window ( widget, window );
#else
    gdk_window_set_user_data ( window, widget );
#endif
}

/******************************************************************************/
GtkWidget *gtk_uvmapeditor_new ( ) {
    GObject *gob = g_object_new ( gtk_uvmapeditor_get_type ( ), NULL );
    GtkUVMapEditor *guv = ( GtkUVMapEditor * ) gob;
    G3DUIUVMAPEDITOR *uvme = &guv->uvme;

    return ( GtkWidget * ) gob;
}

/******************************************************************************/
GtkWidget *createUVMapEditor ( GtkWidget *parent,
                               G3DUI     *gui,
                               char      *name,
                               gint       width,
                               gint       height ) {
    GdkRectangle gdkrec = { 0x00, 0x00, width, height };
    GtkWidget *guv = gtk_uvmapeditor_new ( );
    GtkWidget *area/* = gtk_uvmapeditor_getGLArea ( guv )*/;
    GtkWidget *mbar;

    gtk_widget_set_name ( guv, name );

    gtk_widget_set_size_request ( guv, gdkrec.width, gdkrec.height );

    /*** the OpenGL Window ***/
    area = gtk_drawing_area_new ( );

    gtk_widget_set_double_buffered (area, FALSE );

    /*** For keyboard inputs ***/
    gtk_widget_set_can_focus ( area, TRUE );

    /*** Drawing area does not receive mous events by defaults ***/
    gtk_widget_set_events ( area, gtk_widget_get_events ( area ) |
                                  GDK_KEY_PRESS_MASK             |
			                      GDK_KEY_RELEASE_MASK           |
                                  GDK_BUTTON_PRESS_MASK          |
                                  GDK_BUTTON_RELEASE_MASK        |
                                  GDK_POINTER_MOTION_MASK        |
                                  GDK_POINTER_MOTION_HINT_MASK );
/*
    gdk_window_add_filter ( gtk_widget_get_window ( area ), gdkevent_to_g3devent, view->xevent );
*/

    gtk_fixed_put ( GTK_FIXED(guv), area, 0x00, BUTTONSIZE );

    g_signal_connect ( G_OBJECT (guv), "motion_notify_event" , G_CALLBACK (gtk_uvmapeditor_event), gui );
    g_signal_connect ( G_OBJECT (guv), "button_press_event"  , G_CALLBACK (gtk_uvmapeditor_event), gui );
    g_signal_connect ( G_OBJECT (guv), "button_release_event", G_CALLBACK (gtk_uvmapeditor_event), gui );
    /*g_signal_connect ( G_OBJECT (guv), "configure-event", G_CALLBACK (gtk_uvmapeditor_configure), gui );*/

    /*gdk_window_add_filter ( gtk_widget_get_window ( area ), gtk_area_filter, gui );*/

    /*createObjectMenu           ( area, gui );
    createVertexModeSplineMenu ( area, gui );
    createVertexModeMeshMenu   ( area, gui );
    createEdgeModeMeshMenu     ( area, gui );
    createFaceModeMeshMenu     ( area, gui );
    createSculptModeMeshMenu   ( area, gui );*/

    g_signal_connect ( G_OBJECT (area), "size-allocate"       , G_CALLBACK (g3duiuvmapeditor_sizeGL ), gui );
    g_signal_connect ( G_OBJECT (area), "realize"             , G_CALLBACK (g3duiuvmapeditor_initGL ), gui );
    g_signal_connect ( G_OBJECT (area), "draw"                , G_CALLBACK (g3duiuvmapeditor_showGL ), gui );

    g_signal_connect ( G_OBJECT (area), "motion_notify_event" , G_CALLBACK (g3duiuvmapeditor_inputGL), gui );
    g_signal_connect ( G_OBJECT (area), "key_press_event"     , G_CALLBACK (g3duiuvmapeditor_inputGL), gui );
    g_signal_connect ( G_OBJECT (area), "key_release_event"   , G_CALLBACK (g3duiuvmapeditor_inputGL), gui );
    g_signal_connect ( G_OBJECT (area), "button_press_event"  , G_CALLBACK (g3duiuvmapeditor_inputGL), gui );
    g_signal_connect ( G_OBJECT (area), "button_release_event", G_CALLBACK (g3duiuvmapeditor_inputGL), gui );

    /*** Add the widget to the list uvmap editors, allowing us to ***/
    /*** refresh all of them when an action was done.             ***/
    list_insert ( &gui->luvmapeditor,  guv );

    gtk_widget_show ( area );


    mbar = createUVMapEditorModeBar ( guv, 
                                      gui,
                                      "MODEBAR",
                                      0,
                                      0,
                                      MODEBARBUTTONSIZE,
                                      height );

    gtk_widget_show ( guv );

    gtk_container_add ( GTK_CONTAINER(parent), guv );


    return guv;
}

/******************************************************************************/
void g3duiuvmapeditor_inputGL ( GtkWidget *widget,
                                GdkEvent  *gdkev, 
                                gpointer   user_data ) {
    G3DUI            *gui  = ( G3DUI * ) user_data;
    GdkEventButton   *bev  = ( GdkEventButton * ) gdkev;
    GtkUVMapEditor   *guv  = ( GtkUVMapEditor * ) gtk_widget_get_parent ( widget );
    G3DUIUVMAPEDITOR *uvme = &guv->uvme;
    G3DUIGTK3      *ggt  = ( G3DUIGTK3 * ) gui->toolkit_data;
    static double xold, yold;

    if ( ( gdkev->type == GDK_BUTTON_PRESS ) &&
         ( bev->button == 3 ) ) {
        /*PostMenu ( widget, gdkev, user_data );*/

        return;
    }

    switch ( gdkev->type ) {
        case GDK_KEY_PRESS : {

        } break;

        case GDK_BUTTON_PRESS : {
            if ( gui->flags & VIEWVERTEX ) {
                list_free ( &uvme->lseluv, NULL );

                uvme->lseluv = common_g3duiuvmapeditor_getUV ( uvme,
                                                               gui, 
                                                               bev->x - 4, 
                                                               bev->y - 4,
                                                               0x08,
                                                               0x08 );
            }

            xold = bev->x;
            yold = bev->y;
        } break;

        case GDK_MOTION_NOTIFY : {
            if ( gui->flags & VIEWVERTEX ) {
                LIST *ltmpseluv = uvme->lseluv;

                while ( ltmpseluv ) {
                    G3DUV *uv = ( G3DUV * ) ltmpseluv->data;

                    if ( uvme->canevas.width && uvme->canevas.height ) {
                        uv->u += ( ( float ) ( bev->x - xold ) / uvme->canevas.width  );
                        uv->v += ( ( float ) ( bev->y - yold ) / uvme->canevas.height );
                    }

                    ltmpseluv = ltmpseluv->next;
                }

                gtk_widget_queue_draw ( widget );
            }

            xold = bev->x;
            yold = bev->y;
        } break;

        case GDK_BUTTON_RELEASE : {
            g3dui_redrawGLViews ( gui );

            list_free ( &uvme->lseluv, NULL );
        } break;

        default :
        break;
    }
}

/******************************************************************************/
void g3duiuvmapeditor_sizeGL ( GtkWidget    *widget, 
                               GdkRectangle *allocation, 
                               gpointer      user_data ) {
    GtkUVMapEditor   *guv    = ( GtkUVMapEditor * ) gtk_widget_get_parent ( widget );
    G3DUIUVMAPEDITOR *uvme   = &guv->uvme;
    GdkDisplay       *gdkdpy = gtk_widget_get_display ( widget );
    GdkWindow        *gdkwin = gtk_widget_get_window  ( widget );
    G3DUI            *gui    = ( G3DUI * ) user_data;

    /* 
     * when the top level window is created, the opengl window might not
     * be valid yet. We must check the pointer.
     */
    if ( gdkwin ) {
    #ifdef __linux__
        Display    *dpy    = gdk_x11_display_get_xdisplay ( gdkdpy );
        Window      win    = gdk_x11_window_get_xid       ( gdkwin );

        glXMakeCurrent ( dpy, win, uvme->glctx );
    #endif

    #ifdef __MINGW32__
        HWND hWnd = GDK_WINDOW_HWND ( gdkwin );
        HDC dc = GetDC ( hWnd );

        wglMakeCurrent ( dc, uvme->glctx );

        ReleaseDC ( hWnd, dc );
    #endif
        common_g3duiuvmapeditor_sizeGL ( uvme, allocation->width, 
                                               allocation->height );

        gtk_widget_queue_draw ( widget );
    }
}

/******************************************************************************/
void g3duiuvmapeditor_initGL ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GtkUVMapEditor   *guv      = ( GtkView * ) gtk_widget_get_parent ( widget );
    G3DUIUVMAPEDITOR *uvme     = &guv->uvme;
    GdkWindow        *p_window = gtk_widget_get_parent_window ( widget );
    GdkDisplay       *gdkdpy   = gtk_widget_get_display ( widget );
    GdkWindow        *gdkwin   = gtk_widget_get_window  ( widget );

#ifdef __linux__
    Display    *dis      = gdk_x11_display_get_xdisplay ( gdkdpy );
    Window      root     = DefaultRootWindow ( dis );
    GLint       glattr[] = { GLX_RGBA,
                             GLX_RED_SIZE,    8,
                             GLX_BLUE_SIZE,   8,
                             GLX_GREEN_SIZE,  8,
                             GLX_ALPHA_SIZE,  8,
                             GLX_DEPTH_SIZE, 24,
                             GLX_DOUBLEBUFFER,
                             None };
    static GLXContext shared = NULL;
    G3DUI *gui = ( G3DUI * ) user_data;
    GtkAllocation allocation;
    GdkWindowAttr attr;
    GdkScreen *gdkscr;
    XVisualInfo *vi;
    Window win;
    
    vi = glXChooseVisual ( dis, 0, glattr );

    if ( vi == 0x00 ) {
        fprintf ( stderr, "Could not create GL Visual\n" );

        return;
    }

/**********************/
    gdkscr   = gtk_widget_get_screen  ( widget );

    attr.title       = "OpenGL Window";
    attr.event_mask  = gtk_widget_get_events ( widget ) | GDK_EXPOSURE_MASK       |
                                                          GDK_STRUCTURE_MASK      |
                                                          GDK_BUTTON_PRESS_MASK   | 
                                                          GDK_BUTTON_RELEASE_MASK | 
                                                          GDK_POINTER_MOTION_MASK |
                                                          GDK_POINTER_MOTION_HINT_MASK;

    gtk_widget_get_allocation ( widget, &allocation );

    attr.x           = allocation.x;
    attr.y           = allocation.y;
    attr.width       = allocation.width;
    attr.height      = allocation.height;
    attr.wclass      = GDK_INPUT_OUTPUT;
    attr.window_type = GDK_WINDOW_CHILD;
    attr.visual      = gdk_x11_screen_lookup_visual ( gdkscr, vi->visualid );

    gdkwin = gdk_window_new ( p_window, &attr, GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL );

    gtk_widget_set_window   ( widget, gdkwin );

    gdk_window_set_user_data ( gdkwin, widget );
/*************************/

    win = gdk_x11_window_get_xid ( gdkwin );

    /*** Create OpenGL Context ***/
    uvme->glctx = glXCreateContext( dis, vi, gui->sharedCtx, True );

    if ( gui->sharedCtx == NULL ) gui->sharedCtx = uvme->glctx;

    /*** Set Context as the current context ***/
    glXMakeCurrent ( dis, win, uvme->glctx );

    common_g3duiuvmapeditor_initGL ( uvme );

    /*** Free the memory assigned for GLwNvisualInfo ***/
    free ( vi );
#endif

#ifdef __MINGW32__
    PIXELFORMATDESCRIPTOR pfd;
    HWND hWnd = GDK_WINDOW_HWND ( gdkwin );
    HDC dc = GetDC ( hWnd );
    int pxf;
    
    /*** prevents erasing the background ***/
    gtk_widget_set_double_buffered (GTK_WIDGET (widget), FALSE);
    gtk_widget_set_app_paintable (GTK_WIDGET (widget), TRUE);

    memset ( &pfd, 0x00, sizeof ( PIXELFORMATDESCRIPTOR ) );

    pfd.nSize        = sizeof ( PIXELFORMATDESCRIPTOR );
    pfd.nVersion     = 0x01;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType   = PFD_TYPE_RGBA;
    pfd.cDepthBits   = 0x18;
    pfd.cColorBits   = 0x20;

    if ( ( pxf = ChoosePixelFormat ( dc, &pfd ) ) == 0x00 ) {
        fprintf ( stderr, "ChoosePixelFormat failed\n" );

        return 0x00;
    }

    if ( SetPixelFormat ( dc, pxf, &pfd ) == 0x00 ) {
        fprintf ( stderr, "SetPixelFormat failed\n" );

        return 0x00;
    }

    uvme->glctx = wglCreateContext ( dc );

    wglMakeCurrent ( dc,  uvme->glctx );

    common_g3duiuvmapeditor_initGL ( uvme );

    ReleaseDC ( hWnd, dc );
#endif
}

/******************************************************************************/
gboolean g3duiuvmapeditor_showGL ( GtkWidget *widget, 
                                   cairo_t   *cr,
                                   gpointer   user_data ) {
    GtkUVMapEditor   *guv    = ( GtkUVMapEditor * ) gtk_widget_get_parent ( widget );
    G3DUIUVMAPEDITOR *uvme   = &guv->uvme;
    GdkDisplay       *gdkdpy = gtk_widget_get_display       ( widget );
    GdkWindow        *gdkwin = gtk_widget_get_window        ( widget );
    G3DUI            *gui    = ( G3DUI * ) user_data;
    G3DUIGTK3        *ggt    = ( G3DUIGTK3 * ) gui->toolkit_data;;
    G3DMOUSETOOL     *mou    = uvme->mou;
    uint32_t current;

#ifdef __linux__
    Display      *dpy    = gdk_x11_display_get_xdisplay ( gdkdpy );
    Window        win    = gdk_x11_window_get_xid       ( gdkwin );
#endif

#ifdef __MINGW32__
    HWND hWnd = GDK_WINDOW_HWND ( gdkwin );
    HDC dc = GetDC ( hWnd );
#endif

#ifdef __linux__
    /*** Set Context as the current context ***/
    glXMakeCurrent ( dpy, win, uvme->glctx );

    common_g3duiuvmapeditor_showGL ( uvme, gui );

    glXSwapBuffers ( dpy, win );

    XSync ( dpy, False );
#endif

#ifdef __MINGW32__
    /*** Set Context as the current context ***/
    wglMakeCurrent ( dc, uvme->glctx );

    common_g3duiuvmapeditor_showGL ( uvme, gui );

    SwapBuffers ( dc );

    ReleaseDC ( hWnd, dc );
#endif

    return 0x01;
}

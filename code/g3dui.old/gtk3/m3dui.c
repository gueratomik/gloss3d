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

gboolean m3dui_showGL ( GtkWidget *widget, 
                                   cairo_t   *cr,
                                   gpointer   user_data );
void m3dui_initGL ( GtkWidget *widget, 
                               gpointer   user_data );
void m3dui_sizeGL ( GtkWidget    *widget, 
                               GdkRectangle *allocation, 
                               gpointer      user_data );
static gboolean m3dui_inputGL ( GtkWidget *widget,
                                           GdkEvent  *gdkev, 
                                           gpointer   user_data );
gboolean m3dui_destroyGL ( GtkWidget *widget, 
                                     GdkEvent  *event, 
                                     gpointer   user_data );

/******************************************************************************/
void m3dui_updateMouseToolEdit ( M3DUI *mui ) {
    LIST *ltmpwidget = mui->lmtools;

    while ( ltmpwidget ) {
        GtkWidget *widget = ( GtkWidget * ) ltmpwidget->data;

        updateM3DMouseTool ( widget, mui );

        ltmpwidget = ltmpwidget->next;
    }

    /*g3dui_redrawWidgetList ( mui->gui, mui->lmtools );*/
}

/******************************************************************************/
void m3dui_setUVMouseTool ( GtkWidget *widget, gpointer user_data ) {
    M3DUI *mui = ( M3DUI * ) user_data;
    G3DUI        *gui = mui->gui;
    G3DUIGTK3    *ggt = ( G3DUIGTK3    * ) gui->toolkit_data;
    const char  *name = gtk_widget_get_name ( widget );
    G3DMOUSETOOL *uvmou = common_g3dui_getMouseTool ( gui, name );
    G3DCAMERA *cam = g3dui_getCurrentUVMapEditorCamera ( gui );

    if ( gui->lock ) return;

    if ( uvmou ) {
        common_m3dui_setUVMouseTool ( mui, cam, uvmou );

        if ( ( uvmou->flags & MOUSETOOLNOCURRENT ) == 0x00 ) {
            /*** Remember that widget ID, for example to be unset when a toggle button 
            from another parent widget is called (because XmNradioBehavior won't talk
            to other parent widget ***/
            if ( ggt->currentUVMouseToolButton ) {
                if ( widget != ggt->currentUVMouseToolButton ) {
                    gui->lock = 0x01;

                    if ( GTK_IS_TOGGLE_TOOL_BUTTON ( widget ) ) {
                        GtkToggleToolButton *ttb = GTK_TOGGLE_TOOL_BUTTON ( ggt->currentUVMouseToolButton );

                        gtk_toggle_tool_button_set_active ( ttb, FALSE );
                    }

                    gui->lock = 0x00;
                    /*XtVaSetValues ( ggt->curmou, XmNset, False, NULL );*/
                }
            }
        }

        ggt->currentUVMouseToolButton = widget;

        /*g3dui_updateAllCurrentMouseTools ( gui );*/
    } else {
        fprintf ( stderr, "No such mousetool %s\n", name );
    }

    m3dui_updateMouseToolEdit ( mui );
}

/******************************************************************************/
void m3dui_undoCbk ( GtkWidget *widget, gpointer user_data ) {
    M3DUI *mui = ( M3DUI * ) user_data;

    common_m3dui_undoCbk ( mui );
}

/******************************************************************************/
void m3dui_redoCbk ( GtkWidget *widget, gpointer user_data ) {
    M3DUI *mui = ( M3DUI * ) user_data;

    common_m3dui_redoCbk ( mui );
}

/******************************************************************************/
GtkWidget *gtk_uvmapeditor_getGLArea ( GtkWidget *widget ) {
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
static void gtk_uvmapeditor_class_init ( GtkUVMapEditorClass *lui_class ) {

    GObjectClass   *object_class = G_OBJECT_CLASS   ( lui_class );
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS ( lui_class );
    GdkPixbuf     **icon         = lui_class->icon;

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
    M3DUI *mui = &guv->mui;
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

            mui->buttonID = common_m3dui_getCurrentButton ( mui, bev->x, 
                                                                              bev->y );

            xori = xold = bev->x;
            yori = yold = bev->y;

            if ( ( mui->buttonID > -1 ) && ( grabbing == 0x00 ) ) { 
                gdk_window_set_cursor ( gtk_widget_get_window ( gtk_widget_get_toplevel ( widget ) ), 
                                        gdk_cursor_new ( GDK_BLANK_CURSOR ) );

                gtk_uvmapeditor_grab_pointer ( widget, event );

                grabbing = 0x01;
            }

            gtk_uvmapeditor_redraw_menu ( widget );
        } break;

        case GDK_BUTTON_RELEASE : {
            GdkEventButton *bev = ( GdkEventButton * ) event;

            if ( mui->buttonID > -1 ) {
                switch ( mui->buttonID ) {
                    default:
                        if ( grabbing ) {
                            gtk_uvmapeditor_ungrab_pointer ( widget, event );

                            gdk_window_set_cursor ( gtk_widget_get_window ( gtk_widget_get_toplevel ( widget ) ), 
                                                NULL );

                            grabbing = 0x00;
                        }
                    break;
                }
            }

            /*** Reset selected button flag ***/
            mui->buttonID = -1;

            gtk_uvmapeditor_redraw_menu ( widget );
        } break;

        case GDK_MOTION_NOTIFY : {
            GdkEventMotion *mev = ( GdkEventMotion * ) event;

            /*** If pressed ***/
            if ( mui->buttonID > -1 && grabbing ) {

                switch ( mui->buttonID ) {
                    case UVMAPZOOMBUTTON : {
                        if ( mev->state & GDK_BUTTON1_MASK ) {
                            common_m3dui_moveForward ( mui, 
                                                                  mev->x, 
                                                                  xold );
                        }
                    } break;

                    case UVMAPTRANSLATEBUTTON : {
                        if ( mev->state & GDK_BUTTON1_MASK ) {
                            common_m3dui_moveSideward ( mui,
                                                                   mev->x,
                                                                   mev->y, 
                                                                   xold, 
                                                                   yold );
                        }

                        if ( mev->state & GDK_BUTTON3_MASK ) {
                            common_m3dui_moveForward ( mui, 
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
    M3DUI *mui = &guv->mui;

    common_m3dui_resize ( mui, 
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
            gdkrec.y      += ( TOOLBARBUTTONSIZE + 0x20 );
            gdkrec.width   = MODEBARBUTTONSIZE;
            gdkrec.height  = allocation->height - TOOLBARBUTTONSIZE - 0x20;

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        if ( strcmp ( child_name, "TOOLBAR" ) == 0x00 ) {
            gdkrec.x      += 0;
            gdkrec.y      += 0x20;
            gdkrec.width   = allocation->width;
            gdkrec.height  = TOOLBARBUTTONSIZE;

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        if ( strcmp ( child_name, "MENUBAR" ) == 0x00 ) {
            gdkrec.x      += 0;
            gdkrec.y      += 0;
            gdkrec.width   = allocation->width;
            gdkrec.height  = 0x20;

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        if ( GTK_IS_DRAWING_AREA(child) ) {
            gdkrec.x       = mui->arearec.x;
            gdkrec.y       = mui->arearec.y;
            gdkrec.width   = mui->arearec.width;
            gdkrec.height  = mui->arearec.height;

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        if ( strcmp ( child_name, "PATTERNLIST" ) == 0x00 ) {
            gdkrec.x       = mui->patternrec.x;
            gdkrec.y       = mui->patternrec.y;
            gdkrec.width   = mui->patternrec.width;
            gdkrec.height  = mui->patternrec.height;

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        if ( strcmp ( child_name, "FGBGBUTTON" ) == 0x00 ) {
            gdkrec.x       = mui->fgbgrec.x;
            gdkrec.y       = mui->fgbgrec.y;
            gdkrec.width   = mui->fgbgrec.width;
            gdkrec.height  = mui->fgbgrec.height;

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        if ( strcmp ( child_name, "MOUSETOOL" ) == 0x00 ) {
            gdkrec.x       = mui->toolrec.x;
            gdkrec.y       = mui->toolrec.y;
            gdkrec.width   = mui->toolrec.width;
            gdkrec.height  = mui->toolrec.height;

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        if ( strcmp ( child_name, "CHANNELSELECTION" ) == 0x00 ) {
            gdkrec.x       = MODEBARBUTTONSIZE;
            gdkrec.y       = ( TOOLBARBUTTONSIZE + 0x20 );
            gdkrec.width   = 0x80;
            gdkrec.height  = 0x12;

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
    M3DUI *mui = &guv->mui;

    if ( gtk_widget_is_drawable ( widget ) ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( widget );
        uint32_t width  = gtk_widget_get_allocated_width  ( widget ),
                 height = gtk_widget_get_allocated_height ( widget );
        GtkAllocation alloc;
        uint32_t i;

        gtk_render_background ( context, cr, 0x00, 0x00, width, height );

        for ( i = 0x00; i < NBUVMAPBUTTON; i++ ) {
            GdkPixbuf *icon = guv_class->icon[i];

            if ( i == mui->buttonID ) {
                cairo_set_source_rgb ( cr, 0.25f, 0.25f, 0.25f );

                cairo_rectangle ( cr, mui->rec[i].x, 
                                      mui->rec[i].y, 
                                      mui->rec[i].width, 
                                      mui->rec[i].height );
                cairo_fill ( cr );
            }

            gdk_cairo_set_source_pixbuf ( cr, icon, mui->rec[i].x, 
                                                    mui->rec[i].y );
            cairo_paint ( cr );
        }
    }
 
    return GTK_WIDGET_CLASS (gtk_uvmapeditor_parent_class)->draw (widget, cr);
}

/******************************************************************************/
static void gtk_uvmapeditor_init ( GtkUVMapEditor *guv ) {
    GtkWidget *widget = ( GtkWidget * ) guv;
    M3DUI *mui   = &guv->mui;

    mui->engine_flags = VIEWVERTEXUV;

    /*** Expose event won't be called if we dont set has_window ***/
    gtk_widget_set_has_window ( widget, TRUE );

    common_g3duiview_init ( mui, gtk_widget_get_allocated_width  ( widget ), 
                                  gtk_widget_get_allocated_height ( widget ) );
}

/******************************************************************************/
#define DIFFUSECHANNELSTR      "Diffuse channel"
#define SPECULARCHANNELSTR     "Specular channel"
#define DISPLACEMENTCHANNELSTR "Displacement channel"
#define ALPHACHANNELSTR        "Alpha channel"
#define BUMPCHANNELSTR         "Bump channel"
#define REFLECTIONCHANNELSTR   "Reflection channel"
#define REFRACTIONCHANNELSTR   "Refraction channel"

static void selectChannelCbk( GtkWidget *widget, 
                              gpointer   user_data ) {
    M3DUI *mui = ( M3DUI * ) user_data;
    GtkComboBoxText *cmbt = GTK_COMBO_BOX_TEXT(widget);
    const char *str  = gtk_combo_box_text_get_active_text ( cmbt );

    mui->engine_flags = ( mui->engine_flags & (~UVCHANNELMASK) );

    if ( strcmp ( str, DIFFUSECHANNELSTR ) == 0x00 ) {
        SHOWCHANNEL(mui->engine_flags,DIFFUSECHANNELID);
    }

    if ( strcmp ( str, SPECULARCHANNELSTR ) == 0x00 ) {
        SHOWCHANNEL(mui->engine_flags,SPECULARCHANNELID);
    }

    if ( strcmp ( str, DISPLACEMENTCHANNELSTR ) == 0x00 ) {
        SHOWCHANNEL(mui->engine_flags,DISPLACEMENTCHANNELID);
    }

    if ( strcmp ( str, ALPHACHANNELSTR ) == 0x00 ) {
        SHOWCHANNEL(mui->engine_flags,ALPHACHANNELID);
    }

    if ( strcmp ( str, BUMPCHANNELSTR ) == 0x00 ) {
        SHOWCHANNEL(mui->engine_flags,BUMPCHANNELID);
    }

    if ( strcmp ( str, REFLECTIONCHANNELSTR ) == 0x00 ) {
        SHOWCHANNEL(mui->engine_flags,REFLECTIONCHANNELID);
    }

    if ( strcmp ( str, REFRACTIONCHANNELSTR ) == 0x00 ) {
        SHOWCHANNEL(mui->engine_flags,REFRACTIONCHANNELID);
    }

    /*** resize selection mask and zbuffer ***/
    common_m3dui_resizeBuffers ( mui );
}

/******************************************************************************/
static void createChannelSelection ( GtkWidget        *parent, 
                                     M3DUI *mui,
                                     char             *name,
                                     gint              x,
                                     gint              y,
                                     gint              width ) {
    GtkWidget     *cmb  = gtk_combo_box_text_new ( );
    GdkRectangle   crec = { 0x00, 0x00, width, 0x12 };

    gtk_widget_set_name ( cmb, name );

    gtk_widget_size_allocate ( cmb, &crec );

    gtk_fixed_put ( GTK_FIXED(parent), cmb, x, y );

    g_signal_connect ( cmb, "changed", G_CALLBACK(selectChannelCbk), mui );

    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, DIFFUSECHANNELSTR      );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, SPECULARCHANNELSTR     );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, DISPLACEMENTCHANNELSTR );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, ALPHACHANNELSTR        );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, BUMPCHANNELSTR         );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, REFLECTIONCHANNELSTR   );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, REFRACTIONCHANNELSTR   );

    gtk_combo_box_set_active ( GTK_COMBO_BOX(cmb), 0x00 );

    gtk_widget_show ( cmb );
}

/******************************************************************************/
static void gtk_uvmapeditor_realize ( GtkWidget *widget ) {
    GdkWindow *parent_window = gtk_widget_get_parent_window ( widget );
    GtkStyleContext  *style  = gtk_widget_get_style_context ( widget );
    GtkUVMapEditor *guv = ( GtkUVMapEditor * ) widget;
    M3DUI *mui = &guv->mui;
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

    common_m3dui_init ( mui, allocation.width, allocation.height );

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
    M3DUI *mui = &guv->mui;

    return ( GtkWidget * ) gob;
}

/******************************************************************************/
gboolean gtk_uvmapeditor_destroy ( GtkWidget *widget, gpointer   user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    GtkUVMapEditor *guv = ( GtkUVMapEditor * ) widget;
    G3DUIGTK3      *ggt  = ( G3DUIGTK3 * ) gui->toolkit_data;

    list_remove ( &gui->luvmapeditor,  guv );


    ggt->currentUVMouseToolButton = NULL;

    /*** Free the undo-redo stack ***/
    /*g3durmanager_free ( guv->mui.uvurm );*/

    return FALSE;
}

/******************************************************************************/
GtkWidget *createUVMapEditor ( GtkWidget *parent,
                               G3DUI     *gui,
                               char      *name,
                               gint       width,
                               gint       height ) {
    GdkRectangle gdkrec = { 0x00, 0x00, width, height };
    GtkWidget *guv = gtk_uvmapeditor_new ( );
    M3DUI *mui = &((GtkUVMapEditor*)guv)->mui;
    GtkWidget *area/* = gtk_uvmapeditor_getGLArea ( guv )*/;
    GtkWidget *mbar;
    GtkWidget *tbar;

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

    gtk_fixed_put ( GTK_FIXED(guv), area, 0x00, BUTTONSIZE + 0x20 );

    g_signal_connect ( G_OBJECT (guv), "motion_notify_event" , G_CALLBACK (gtk_uvmapeditor_event), gui );
    g_signal_connect ( G_OBJECT (guv), "button_press_event"  , G_CALLBACK (gtk_uvmapeditor_event), gui );
    g_signal_connect ( G_OBJECT (guv), "button_release_event", G_CALLBACK (gtk_uvmapeditor_event), gui );
    g_signal_connect ( G_OBJECT (guv), "destroy"       , G_CALLBACK (gtk_uvmapeditor_destroy), gui );

    /*g_signal_connect ( G_OBJECT (guv), "configure-event", G_CALLBACK (gtk_uvmapeditor_configure), gui );*/

    /*gdk_window_add_filter ( gtk_widget_get_window ( area ), gtk_area_filter, gui );*/

    /*createObjectMenu           ( area, gui );
    createVertexModeSplineMenu ( area, gui );
    createVertexModeMeshMenu   ( area, gui );
    createEdgeModeMeshMenu     ( area, gui );
    createFaceModeMeshMenu     ( area, gui );
    createSculptModeMeshMenu   ( area, gui );*/

    g_signal_connect ( G_OBJECT (area), "size-allocate"       , G_CALLBACK (m3dui_sizeGL    ), gui );
    g_signal_connect ( G_OBJECT (area), "realize"             , G_CALLBACK (m3dui_initGL    ), gui );
    g_signal_connect ( G_OBJECT (area), "draw"                , G_CALLBACK (m3dui_showGL    ), gui );
    g_signal_connect ( G_OBJECT (area), "destroy-event"       , G_CALLBACK (m3dui_destroyGL ), gui );

    g_signal_connect ( G_OBJECT (area), "motion_notify_event" , G_CALLBACK (m3dui_inputGL), gui );
    g_signal_connect ( G_OBJECT (area), "key_press_event"     , G_CALLBACK (m3dui_inputGL), gui );
    g_signal_connect ( G_OBJECT (area), "key_release_event"   , G_CALLBACK (m3dui_inputGL), gui );
    g_signal_connect ( G_OBJECT (area), "button_press_event"  , G_CALLBACK (m3dui_inputGL), gui );
    g_signal_connect ( G_OBJECT (area), "button_release_event", G_CALLBACK (m3dui_inputGL), gui );

    /*** Add the widget to the list uvmap editors, allowing us to ***/
    /*** refresh all of them when an action was done.             ***/
    list_insert ( &gui->luvmapeditor,  guv );

    gtk_widget_show ( area );

    ((GtkUVMapEditor*)guv)->mui.gui = gui;

    mbar = createUVMapEditorModeBar ( guv, 
                                      gui,
                                      "MODEBAR",
                                      0,
                                      0,
                                      MODEBARBUTTONSIZE,
                                      height );

    tbar = createUVMapEditorToolBar ( guv, 
                                      gui,
                                      "TOOLBAR",
                                      0,
                                      0,
                                      width,
                                      32 );

    createUVMenuBar   ( guv, mui, "MENUBAR", 0x00,
                                               0x00,
                                               width,
                                               32 );

    createPatternList ( guv, mui, "PATTERNLIST", 0x00,
                                                  0x00,
                                                  256,
                                                  128 );

    createChannelSelection ( guv, mui, "CHANNELSELECTION", 0x00, 0x00, 0x80 );

    createFgBgButton ( guv, gui, "FGBGBUTTON", 0x00, 0x00, 0x30, 0x30 );
    createM3DMouseToolEdit ( guv, mui, "MOUSETOOL", 0x00, 0x00, 256, 256 );

    gtk_widget_show ( guv );

    gtk_container_add ( GTK_CONTAINER(parent), guv );

    /*** size mask and z buffers ***/
    common_m3dui_resizeBuffers ( mui );


    return guv;
}

/******************************************************************************/
static gboolean m3dui_inputGL ( GtkWidget *widget,
                                           GdkEvent  *gdkev, 
                                           gpointer   user_data ) {
    static int i;
    G3DUI            *gui  = ( G3DUI * ) user_data;

    GdkEventButton   *bev  = ( GdkEventButton * ) gdkev;
    GtkUVMapEditor   *guv  = ( GtkUVMapEditor * ) gtk_widget_get_parent ( widget );
    M3DUI *mui = &guv->mui;
    G3DUIGTK3      *ggt  = ( G3DUIGTK3 * ) gui->toolkit_data;
    static double xold, yold;
    GdkDisplay     *gdkdpy = gtk_widget_get_display ( widget );
    GdkWindow      *gdkwin = gtk_widget_get_window  ( widget );

#ifdef __linux__
    Display    *dpy    = gdk_x11_display_get_xdisplay ( gdkdpy );
    Window      win    = gdk_x11_window_get_xid       ( gdkwin );

    glXMakeCurrent ( dpy, win, mui->glctx );
#endif

#ifdef __MINGW32__
    HWND hWnd = GDK_WINDOW_HWND ( gdkwin );
    HDC dc = GetDC ( hWnd );

    wglMakeCurrent ( dc, mui->glctx );

    ReleaseDC ( hWnd, dc );
#endif


    if ( ( gdkev->type == GDK_BUTTON_PRESS ) &&
         ( bev->button == 3 ) ) {
        /*PostMenu ( widget, gdkev, user_data );*/

        return 0;
    }

    switch ( gdkev->type ) {
        case GDK_KEY_PRESS : {
            GdkEventKey *kev = ( GdkEventKey * ) gdkev;

            switch ( kev->keyval ) {
                case GDK_KEY_Delete: {
                    /*common_g3dui_deleteSelectionCbk ( gui );*/
                } break;

                case GDK_KEY_z: {
                    /*** undo ***/
                    if ( kev->state & GDK_CONTROL_MASK ) {
                        common_g3dui_undoCbk ( gui );
                    }
                } break;

                case GDK_KEY_y: {
                    /*** redo ***/
                    if ( kev->state & GDK_CONTROL_MASK ) {
                        common_g3dui_redoCbk ( gui );
                    }
                } break;

                /*case GDK_KEY_c: {
                    common_g3dui_copySelectionCbk ( gui );
                } break;

                case GDK_KEY_v: {
                    common_g3dui_pasteSelectionCbk ( gui );
                } break;

                case GDK_KEY_a: {
                    common_g3dui_selectAllCbk ( gui );
                } break;*/
            }
        } break;

        case GDK_BUTTON_PRESS : {
/*
            G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, ( uint64_t ) widget );
*/
            /*** If there was a running render, cancel it and dont go further ***/
            /*if ( rps ) {
                r3dscene_cancelRender ( rps->rsce );

                return;
            }*/

            /*** For keyboard inputs ***/
            /*gtk_widget_grab_focus ( widget );*/
        } break;

        default :
        break;
    }

    if ( gui->uvmou ) {
        /*** Call the mousetool callback and redraw if return value is > 0 ***/
        G3DEvent g3dev;

        gdkevent_to_g3devent ( gdkev, &g3dev );

        gui->uvmou->mask    = mui->mask;
        gui->uvmou->zbuffer = mui->zbuffer;

        if ( gui->uvmou->tool ) {
            uint32_t msk = gui->uvmou->tool ( gui->uvmou, 
                                              gui->sce,
                                              &mui->cam,
                                              /*mui->uvurm*/gui->urm,
                                              mui->engine_flags, &g3dev );

            common_g3dui_interpretMouseToolReturnFlags ( gui, msk );

            if ( gdkev->type == GDK_BUTTON_RELEASE ) {
                g3dcursor_reset ( &gui->sce->csr );
            }
        }
    }

    return TRUE;
}

/******************************************************************************/
gboolean m3dui_destroyGL ( GtkWidget *widget, 
                                      GdkEvent  *event, 
                                      gpointer   user_data ) {
    GtkUVMapEditor   *guv    = ( GtkUVMapEditor * ) gtk_widget_get_parent ( widget );
    M3DUI *mui   = &guv->mui;
    GdkDisplay       *gdkdpy = gtk_widget_get_display ( widget );
    GdkWindow        *gdkwin = gtk_widget_get_window  ( widget );
    G3DUI            *gui    = ( G3DUI * ) user_data;

    common_m3dui_destroyGL ( mui );

    /*list_remove ( &gui->luvmapeditor,  guv );*/

    return FALSE;
}

/******************************************************************************/
void m3dui_sizeGL ( GtkWidget    *widget, 
                               GdkRectangle *allocation, 
                               gpointer      user_data ) {
    GtkUVMapEditor   *guv    = ( GtkUVMapEditor * ) gtk_widget_get_parent ( widget );
    M3DUI *mui   = &guv->mui;
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

        glXMakeCurrent ( dpy, win, mui->glctx );
    #endif

    #ifdef __MINGW32__
        HWND hWnd = GDK_WINDOW_HWND ( gdkwin );
        HDC dc = GetDC ( hWnd );

        wglMakeCurrent ( dc, mui->glctx );

        ReleaseDC ( hWnd, dc );
    #endif
        common_m3dui_sizeGL ( mui, allocation->width, 
                                               allocation->height );

        gtk_widget_queue_draw ( widget );
    }
}

/******************************************************************************/
void m3dui_initGL ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GtkUVMapEditor   *guv      = ( GtkView * ) gtk_widget_get_parent ( widget );
    M3DUI *mui     = &guv->mui;
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
    mui->glctx = glXCreateContext( dis, vi, gui->sharedCtx, True );

    if ( gui->sharedCtx == NULL ) gui->sharedCtx = mui->glctx;

    /*** Set Context as the current context ***/
    glXMakeCurrent ( dis, win, mui->glctx );

    common_m3dui_initGL ( mui );

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

    mui->glctx = wglCreateContext ( dc );

    if ( mui->gui->sharedCtx == NULL ) mui->gui->sharedCtx = mui->glctx;
    else wglShareLists( mui->gui->sharedCtx, mui->glctx );
	
    wglMakeCurrent ( dc,  mui->glctx );

    common_m3dui_initGL ( mui );

    ReleaseDC ( hWnd, dc );
#endif
}

/******************************************************************************/
gboolean m3dui_showGL ( GtkWidget *widget, 
                                   cairo_t   *cr,
                                   gpointer   user_data ) {
    GtkUVMapEditor   *guv    = ( GtkUVMapEditor * ) gtk_widget_get_parent ( widget );
    M3DUI *mui   = &guv->mui;
    GdkDisplay       *gdkdpy = gtk_widget_get_display       ( widget );
    GdkWindow        *gdkwin = gtk_widget_get_window        ( widget );
    G3DUI            *gui    = ( G3DUI * ) user_data;
    G3DUIGTK3        *ggt    = ( G3DUIGTK3 * ) gui->toolkit_data;;
    G3DMOUSETOOL     *mou    = gui->uvmou;
    uint32_t current;

    ggt->currentUVMapEditor = guv;

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
    glXMakeCurrent ( dpy, win, mui->glctx );

    common_m3dui_showGL ( mui, gui, gui->uvmou, mui->engine_flags );

    glXSwapBuffers ( dpy, win );

    XSync ( dpy, False );
#endif

#ifdef __MINGW32__
    /*** Set Context as the current context ***/
    wglMakeCurrent ( dc, mui->glctx );

    common_m3dui_showGL ( mui, gui, gui->uvmou, mui->engine_flags );

    SwapBuffers ( dc );

    ReleaseDC ( hWnd, dc );
#endif

    return TRUE;
}

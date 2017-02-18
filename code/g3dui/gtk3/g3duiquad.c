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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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

G_DEFINE_TYPE (GtkQuad, gtk_quad, GTK_TYPE_FIXED)

/******************************************************************************/
static void     gtk_quad_class_init    ( GtkQuadClass * );
static void     gtk_quad_init          ( GtkQuad * );
static void     gtk_quad_realize       ( GtkWidget * );
static void     gtk_quad_size_request  ( GtkWidget *, GtkRequisition * );
static gboolean gtk_quad_expose        ( GtkWidget *, cairo_t * );
static gboolean gtk_quad_configure     ( GtkWidget *, GdkEventConfigure * );
/*static void     gtk_quad_size_allocate ( GtkWidget *, GtkAllocation * );*/
static void     gtk_quad_show          ( GtkWidget * );
static void     gtk_quad_add           ( GtkContainer *, GtkWidget * );

/******************************************************************************/
static void gtk_quad_class_init ( GtkQuadClass *quad_class ) {

    GObjectClass      *object_class    = G_OBJECT_CLASS      ( quad_class );
    GtkWidgetClass    *widget_class    = GTK_WIDGET_CLASS    ( quad_class );
    GtkContainerClass *container_class = GTK_CONTAINER_CLASS ( quad_class );

  /*object_class->set_property  = pfx_foo_set_property;
  object_class->get_property  = pfx_foo_get_property;*/

    widget_class->realize          = gtk_quad_realize;
    widget_class->configure_event  = gtk_quad_configure;
    widget_class->size_allocate    = gtk_quad_size_allocate;
    /*widget_class->adjust_size_request   = gtk_quad_adjust_size_request;*/
    /*widget_class->show             = gtk_quad_show;*/
    widget_class->draw             = gtk_quad_expose;

    /*container_class->add           = gtk_quad_add;*/

  /* ajout de la propriété PfxFoo:active */
  /*g_object_class_install_property (object_class, PROP_ACTIVE,
                                   g_param_spec_boolean ("active",
                                                         "Active",
                                                         "Whether the display is active",
                                                         FALSE,
                                                         G_PARAM_READWRITE));*/
   
  /* ajout d'un élément privé (utilisé pour contenit les champs privés de
   * l'objet) */
  /*g_type_class_add_private (klass, sizeof (PfxFooPrivate));*/
}

/******************************************************************************/
/*static void gtk_quad_show ( GtkWidget *widget ) {
    GTK_WIDGET_GET_CLASS (widget)->show (widget);
}*/

/******************************************************************************/
void gtk_quad_size_allocate ( GtkWidget *widget, GtkAllocation *allocation ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    GtkQuad *gqw = ( GtkQuad * ) widget;
    G3DUIQUAD *quad = &gqw->quad;
    int i;

    common_g3duiquad_resize ( quad, allocation->width, 
                                    allocation->height );

    gtk_widget_set_allocation ( widget, allocation );

    if ( gtk_widget_get_realized ( widget ) ) {
        gdk_window_move_resize ( gtk_widget_get_window ( widget ),
                                 allocation->x,
                                 allocation->y,
                                 allocation->width, 
                                 allocation->height );
    }

    if ( gqw->maximized_view ) {
        GdkRectangle gdkrec;

        for ( i = 0x00; i < 0x04, children; i++, children = g_list_next ( children ) ) {
            GtkWidget *child = ( GtkWidget * ) children->data;

            gdkrec.x      = quad->seg[0x04].x1;
            gdkrec.y      = quad->seg[0x04].y1;
            gdkrec.width  = quad->seg[0x04].x2 - quad->seg[0x04].x1;
            gdkrec.height = quad->seg[0x04].y2 - quad->seg[0x04].y1;

            if ( gtk_widget_get_has_window ( widget ) == 0x00 ) {
                gdkrec.x += allocation->x;
                gdkrec.y += allocation->y;
            }

            if ( child == gqw->maximized_view ) {
                gtk_widget_size_allocate ( child, &gdkrec );
            } else {
                gtk_widget_size_allocate ( child, &gdkrec );

                gtk_widget_hide ( child );
            }
        }
    } else {
        GdkRectangle gdkrec;

        for ( i = 0x00; i < 0x04, children; i++, children = g_list_next ( children ) ) {
            GtkWidget *child = ( GtkWidget * ) children->data;

            if ( gtk_widget_get_visible ( child ) == FALSE ) {
                gtk_widget_show ( child );
            }

            gdkrec.x      = quad->seg[i].x1;
            gdkrec.y      = quad->seg[i].y1;
            gdkrec.width  = quad->seg[i].x2 - quad->seg[i].x1;
            gdkrec.height = quad->seg[i].y2 - quad->seg[i].y1;

            if ( ( gdkrec.width > 0x00 ) && ( gdkrec.height > 0x00 ) ) {
                gtk_widget_size_allocate ( child, &gdkrec );
            }
        }
    }
}

/******************************************************************************/
static gboolean gtk_quad_configure ( GtkWidget *widget,
                                       GdkEventConfigure *event ) {
    /*** use parent class configure function ***/
    GTK_WIDGET_CLASS(gtk_quad_parent_class)->configure_event ( widget, event );


    return 0x01;
}

/******************************* Draw child border ****************************/
static void gtk_quad_draw_tile ( GtkStyleContext *context, cairo_t *cr, 
                                                           G3DUISEGMENT *seg ) {
    /*** retrieve the original rectangle ***/
    int x1 = seg->x1 - MARGIN,
        y1 = seg->y1 - MARGIN,
        x2 = seg->x2 + MARGIN,
        y2 = seg->y2 + MARGIN;

    gtk_render_frame ( context, cr,
                                x1,
                                y1,
                                x2 - x1, 
                                y2 - y1 );
}

/******************************************************************************/
static gboolean gtk_quad_expose ( GtkWidget *widget, cairo_t *cr ) {
    GtkQuad *gqw = ( GtkQuad * ) widget;
    G3DUIQUAD *quad = &gqw->quad;

    if ( gtk_widget_is_drawable ( widget ) ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( widget );

        if ( gqw->maximized_view ) {
            gtk_quad_draw_tile ( context, cr, &quad->seg[0x04] );
        } else {
            uint32_t i;

            for ( i = 0x00; i < 0x04; i++ ) {
                G3DUISEGMENT *seg = &quad->seg[i];

                gtk_quad_draw_tile ( context, cr, seg );
            }
        }
    }

    return GTK_WIDGET_CLASS (gtk_quad_parent_class)->draw (widget, cr);
}

/******************************************************************************/
static void gtk_quad_init ( GtkQuad *gqw ) {
    GtkWidget *widget = ( GtkWidget * ) gqw;
    GtkStyleContext *context;

    context = gtk_widget_get_style_context ( widget );

    /*** Render with button style ***/
    gtk_style_context_add_class ( context, GTK_STYLE_CLASS_BUTTON );

    /*** Expose event won't be called if we dont set has_window ***/
    gtk_widget_set_has_window ( widget, TRUE );

    common_g3duiquad_init ( &gqw->quad, gtk_widget_get_allocated_width  ( widget ), 
                                 gtk_widget_get_allocated_height ( widget ) );
}

/******************************************************************************/
static void gtk_quad_createDefaultViews ( GtkWidget *widget, G3DUI *gui ) {
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;;
    GtkQuad *gqw = ( GtkQuad * ) widget;
    G3DUIQUAD *quad = &gqw->quad;
    G3DCAMERA **cam;
    uint32_t i;

    /*gui->defcam = gui->curcam = cam[0x00];*/

    cam = common_g3dui_createDefaultCameras ( gui );

    for ( i = 0x00; i < 0x04; i++ ) {
        uint32_t vwidth  = ( quad->seg[i].x2 - quad->seg[i].x1 ),
                 vheight = ( quad->seg[i].y2 - quad->seg[i].y1 );
        GtkWidget *gvw;

        /*** Create OpenGL Views ***/
        gvw = createView ( widget, gui, "View", cam[i], vwidth, vheight );

        if ( gui->curcam == NULL ) gui->curcam = cam[i];
        if ( ggt->curogl == NULL ) ggt->curogl = gvw;

        gtk_fixed_put ( GTK_FIXED(widget), gvw, quad->seg[i].x1, quad->seg[i].y1 );


        gtk_widget_show ( gvw );
    }
}

/******************************************************************************/
static void gtk_quad_realize ( GtkWidget *widget ) {
    GdkWindow *parent_window = gtk_widget_get_parent_window ( widget );
    GtkStyleContext  *style  = gtk_widget_get_style_context ( widget );
    GdkWindowAttr attributes;
    GtkAllocation allocation;
    GdkWindow       *window;

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

    attributes.event_mask  |=  ( GDK_EXPOSURE_MASK     |
                                 GDK_KEY_PRESS_MASK    |
			         GDK_KEY_RELEASE_MASK  |
			         GDK_ENTER_NOTIFY_MASK |
			         GDK_LEAVE_NOTIFY_MASK |
			         GDK_STRUCTURE_MASK    |
			         GDK_FOCUS_CHANGE_MASK );

    window = gdk_window_new ( parent_window, &attributes, GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL );

    gtk_widget_set_window   ( widget, window );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_widget_register_window ( widget, window );
#else
    gdk_window_set_user_data ( window, widget );
#endif
}

/******************************************************************************/
GtkWidget *gtk_quad_new ( ) {
  return ( GtkWidget * ) g_object_new ( gtk_quad_get_type ( ),
                                        /*"active", NULL,*/
                                        NULL);
}

/******************************************************************************/
GtkWidget *createQuad ( GtkWidget *parent, G3DUI *gui,
                                           char *name,
                                           gint x,
                                           gint y,
                                           gint width,
                                           gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *gqw = gtk_quad_new ( );

    gtk_widget_set_name ( gqw, name );

    gtk_widget_size_allocate ( gqw, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), gqw, x, y );

    gtk_quad_createDefaultViews ( gqw, gui );

    gtk_widget_show ( gqw );


    return gqw;
}

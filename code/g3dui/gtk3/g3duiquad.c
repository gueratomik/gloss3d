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
static GTK3G3DUIQUAD *g3tk_g3duiquad_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memSize =  sizeof ( GTK3G3DUIQUAD );
    GTK3G3DUIQUAD *gtk3quad = ( GTK3G3DUIQUAD * ) calloc ( 0x01, memSize );

    if ( gtk3quad == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3quad->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3quad;
}

/******************************************************************************/
/*static void SizeAllocate ( GtkWidget     *widget,
                           GtkAllocation *allocation,
                           gpointer       user_data ) {*/
static gboolean SizeAllocate ( GtkWidget        *self,
                               GdkEventConfigure event,
                               gpointer          user_data ) {
    GTK3G3DUIQUAD *gtk3quad = ( GTK3G3DUIQUAD *    ) user_data;

#ifdef unused
    g3duiquad_resize ( &gtk3quad->core, 
                        event.width, 
                        event.height );

printf("quad %d %d\n", event.width, event.height );

    if ( gtk3quad->core.magnified == NULL ) {
        uint32_t i;

        for ( i = 0x00; i < 0x04; i++ ) {
            GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) gtk3quad->core.view[i];

            if ( gtk3view ) {
                GdkRectangle gdkrec = { gtk3quad->core.rect[i].x,
                                        gtk3quad->core.rect[i].y,
                                        gtk3quad->core.rect[i].width,
                                        gtk3quad->core.rect[i].height };

                gtk_widget_size_allocate ( gtk3view->layout, &gdkrec );
            }
        }
    } else {
        GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) gtk3quad->core.magnified;

        if ( gtk3view ) {
            GdkRectangle gdkrec = { 0x00,
                                    0x00,
                                    gtk3quad->core.width,
                                    gtk3quad->core.height };

            gtk_widget_size_allocate ( gtk3view->layout, &gdkrec );
        }
    }
#endif
    return FALSE;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3G3DUIQUAD *gtk3quad = ( GTK3G3DUIQUAD * ) user_data;

    free ( gtk3quad );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3G3DUIQUAD *gtk3quad = ( GTK3G3DUIQUAD * ) user_data;
    G3DUI *gui = gtk3quad->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    g3duiquad_init ( &gtk3quad->core );
}

/******************************************************************************/
static gboolean Draw ( GtkWidget *widget,
                       cairo_t   *cr,
                       gpointer   user_data ) {
    GTK3G3DUIQUAD *gtk3quad = ( GTK3G3DUIQUAD * ) user_data;
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    GdkRGBA color;


/*cairo_region_t * cairoRegion = cairo_region_create();
GdkWindow *window = gtk_layout_get_bin_window (widget);

GdkDrawingContext * drawingContext;
drawingContext = gdk_window_begin_draw_frame (window,cairoRegion);*/

    if ( gtk3quad->core.magnified == NULL ) {
        uint32_t i;

        for ( i = 0x00; i < 0x04; i++ ) {
            GdkRectangle gdkrec = { gtk3quad->core.rect[i].x,
                                    gtk3quad->core.rect[i].y,
                                    gtk3quad->core.rect[i].width,
                                    gtk3quad->core.rect[i].height };
            G3DUI *gui = gtk3quad->core.gui;

            if ( gui->currentView == gtk3quad->core.view[i] ) {
                cairo_set_source_rgb ( cr, 0.2f, 0.2f, 0.2f );

                cairo_rectangle ( cr,
                                  gdkrec.x,
                                  gdkrec.y,
                                  gdkrec.width, 
                                  gdkrec.height );
                cairo_stroke(cr);
            }

/*
cairo_move_to(cr, 30, 30);
                cairo_set_font_size(cr,15);
                cairo_show_text(cr, "hello world");
*/
/*
            gtk_render_frame ( context, cr,
                                        gdkrec.x,
                                        gdkrec.y,
                                        gdkrec.width, 
                                        gdkrec.height );
*/

        }
    } else {
        GdkRectangle gdkrec = { 0x00,
                                0x00,
                                gtk3quad->core.width,
                                gtk3quad->core.height };

        gtk_render_frame ( context, cr,
                                    gdkrec.x,
                                    gdkrec.y,
                                    gdkrec.width, 
                                    gdkrec.height );
    }

/*gdk_window_end_draw_frame(window,drawingContext);

cairo_region_destroy(cairoRegion);*/


    return FALSE;
}

/******************************************************************************/
void gtk3_g3duiquad_rearrange ( GTK3G3DUIQUAD *gtk3quad,
                                GTK3G3DUIVIEW *magnified ) {
    G3DUIQUAD *quad = ( GTK3G3DUIQUAD * ) gtk3quad;
    GtkAllocation qalloc = { 0x00,
                             0x00,
                             gtk3quad->core.width,
                             gtk3quad->core.height };
    int i;

    gtk_widget_get_allocation ( gtk3quad->layout, &qalloc );

    if ( quad->magnified == magnified ) {
        for ( i = 0x00; i < 0x04; i++ ) {
            GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) gtk3quad->core.view[i];

            if ( gtk3view != magnified ) gtk_widget_show ( gtk3view->layout );
        }


        quad->magnified = NULL;
    } else {
        for ( i = 0x00; i < 0x04; i++ ) {
            GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) gtk3quad->core.view[i];

            if ( gtk3view != magnified ) gtk_widget_hide ( gtk3view->layout );
        }

        quad->magnified = magnified;
    }

    gtk3_g3duiquad_resize ( gtk3quad,
                            gtk3quad->core.width,
                            gtk3quad->core.height );
}

/******************************************************************************/
static void gtk3_g3duiquad_createViews ( GTK3G3DUIQUAD *gtk3quad ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3quad->core.gui;
    void (*grid[0x04])(uint64_t) = { g3dcore_grid3D,
                                     g3dcore_gridXY,
                                     g3dcore_gridYZ,
                                     g3dcore_gridZX };
    int i;

    for ( i = 0x00; i < 0x04; i++ ) {
        GTK3G3DUIVIEW *gtk3view = gtk3_g3duiview_create ( gtk3quad->layout,
                                                          gtk3gui,
                                                          "view" );

        list_insert ( &gtk3gui->core.lview, gtk3view );

        gtk3quad->core.view[i] = ( G3DUIVIEW * ) gtk3view;

        gtk3view->core.grid = grid[i];

        gtk_layout_put ( GTK_LAYOUT(gtk3quad->layout), gtk3view->layout, 0, 0 );
    }
}

/******************************************************************************/
void gtk3_g3duiquad_resize ( GTK3G3DUIQUAD *gtk3quad,
                             uint32_t       width,
                             uint32_t       height ) {

    g3duiquad_resize ( &gtk3quad->core, 
                        width, 
                        height );

/*printf("quad %d %d\n", width, height );*/

    if ( gtk3quad->core.magnified == NULL ) {
        uint32_t i;

        for ( i = 0x00; i < 0x04; i++ ) {
            GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) gtk3quad->core.view[i];

            if ( gtk3view ) {
                GdkRectangle gdkrec = { gtk3quad->core.rect[i].x + 0x02,
                                        gtk3quad->core.rect[i].y + 0x02,
                                        gtk3quad->core.rect[i].width  - 0x04,
                                        gtk3quad->core.rect[i].height - 0x04 };

                gtk3_g3duiview_resize ( gtk3view, gdkrec.width, gdkrec.height );

                gtk_layout_move ( gtk3quad->layout,
                                 gtk3view->layout, 
                                 gdkrec.x,
                                 gdkrec.y );

                gtk_widget_set_size_request ( gtk3view->layout,
                                              gdkrec.width,
                                              gdkrec.height );
            }
        }
    } else {
        GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) gtk3quad->core.magnified;

        if ( gtk3view ) {
            GdkRectangle gdkrec = { 0x00 + gtk3quad->core.margin,
                                    0x00 + gtk3quad->core.margin,
                                    gtk3quad->core.width  - gtk3quad->core.margin - gtk3quad->core.margin,
                                    gtk3quad->core.height - gtk3quad->core.margin - gtk3quad->core.margin };

            gtk3_g3duiview_resize ( gtk3view, gdkrec.width, gdkrec.height );

            gtk_layout_move ( gtk3quad->layout,
                             gtk3view->layout, 
                             gdkrec.x,
                             gdkrec.y );

            gtk_widget_set_size_request ( gtk3view->layout,
                                          gdkrec.width,
                                          gdkrec.height );
        }
    }

    return FALSE;
}

/******************************************************************************/
GTK3G3DUIQUAD *gtk3_g3duiquad_create ( GtkWidget *parent,
                                       GTK3G3DUI *gtk3gui,
                                       char      *name ) {
    G3DUI *gui = &gtk3gui->core;
    GTK3G3DUIQUAD *gtk3quad = g3tk_g3duiquad_new ( gtk3gui );
    GtkWidget    *layout  = gtk_layout_new ( NULL, NULL );

    gtk3quad->layout = layout;

    gtk_widget_set_name ( layout, name );

    gtk_widget_add_events(GTK_WIDGET(layout), GDK_CONFIGURE);

    g_signal_connect ( G_OBJECT (layout), "realize"      , G_CALLBACK (Realize)     , gtk3quad );
    g_signal_connect ( G_OBJECT (layout), "destroy"      , G_CALLBACK (Destroy)     , gtk3quad );
    g_signal_connect ( G_OBJECT (layout), "draw"         , G_CALLBACK (Draw)        , gtk3quad );

    gtk3_g3duiquad_createViews ( gtk3quad ) ;

    gtk_widget_show ( gtk3quad->layout );


    return gtk3quad;
}


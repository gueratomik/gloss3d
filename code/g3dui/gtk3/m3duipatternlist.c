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
static
void gtk3_m3duipatternpreview_free ( GTK3M3DUIPATTERNPREVIEW *gtk3patpreview ) {
    g_object_unref ( gtk3patpreview->img );

    free ( gtk3patpreview );
}

/******************************************************************************/
static void DrawPreview ( GtkWidget               *widget,
                          GTK3M3DUIPATTERNLIST    *gtk3patlist,
                          cairo_t                 *cr,
                          GTK3M3DUIPATTERNPREVIEW *gtk3patpreview,
                          uint32_t                 selected ) {

    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    GdkRGBA fg;

    gtk_style_context_get_color ( context, GTK_STATE_FLAG_NORMAL, &fg );

    gtk_render_frame ( context, 
                       cr,
                       gtk3patpreview->core.rec.x,
                       gtk3patpreview->core.rec.y, 
                       gtk3patpreview->core.rec.width, 
                       gtk3patpreview->core.rec.height );

    if ( selected ) {
        cairo_set_source_rgb ( cr, 0.0f, 0.0f, 1.0f );
        cairo_rectangle ( cr, gtk3patpreview->core.rec.x,
                              gtk3patpreview->core.rec.y,
                              gtk3patpreview->core.rec.width,
                              gtk3patpreview->core.rec.height );
        cairo_set_line_width ( cr, 1.0f );
        cairo_stroke ( cr );
    }

    /*** Draw the texture preview ***/
    gdk_cairo_set_source_pixbuf ( cr, 
                                  gtk3patpreview->img, 
                                  gtk3patpreview->core.rec.x + 
                                  gtk3patlist->core.preview_border,
                                  gtk3patpreview->core.rec.y + 
                                  gtk3patlist->core.preview_border );

    cairo_paint ( cr );
}

/******************************************************************************/
static void Draw ( GtkWidget *widget, 
                   cairo_t   *cr, 
                   gpointer   user_data ) {
    GTK3M3DUIPATTERNLIST *gtk3patlist = ( GTK3M3DUIPATTERNLIST * ) user_data;
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    LIST *ltmppreview = gtk3patlist->core.lpreview;
    GtkAllocation allocation;
    uint32_t x = 0x00, y = 0x00;
    uint32_t maxheight = 0x00;
    M3DUI *mui = ( M3DUI * ) gtk3patlist->core.mui;

    gtk_widget_get_allocation ( widget, &allocation );

    gtk_render_background ( context,
                            cr,
                            0x00,
                            0x00,
                            allocation.width,
                            allocation.height );

    while ( ltmppreview ) {
        GTK3M3DUIPATTERNPREVIEW *gtk3patpreview = ( GTK3M3DUIPATTERNPREVIEW * ) ltmppreview->data;
        uint32_t selected = 0x00;

        if ( gtk3patpreview->core.pat == mui->selpat ) {
            selected = 0x01;
        }

        DrawPreview ( widget, gtk3patlist, cr, gtk3patpreview, selected );

        ltmppreview = ltmppreview->next;
    }
}

/******************************************************************************/
static void Input ( GtkWidget *widget,
                    GdkEvent  *gdkev, 
                    gpointer   user_data ) {
    GTK3M3DUIPATTERNLIST *gtk3patlist = ( GTK3M3DUIPATTERNLIST * ) user_data;
    M3DUI *mui = ( M3DUI * ) gtk3patlist->core.mui;
    G3DUI *gui = ( G3DUI * ) mui->gui;

    switch ( gdkev->type ) {
        case GDK_BUTTON_PRESS : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;
            GTK3M3DUIPATTERNPREVIEW *gtk3patpreview;

            gtk3patpreview = ( GTK3M3DUIPATTERNPREVIEW * ) m3duipatternlist_pickPreview ( &gtk3patlist->core, 
                                                                                           bev->x,
                                                                                           bev->y );

            if ( gtk3patpreview ) {
                mui->selpat = gtk3patpreview->core.pat;
            }
#ifdef TODO
            gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW          |
                                                   REDRAWMATERIALLIST  | 
                                                   UPDATECURRENTMATERIAL );
#endif
        } break;

        default:
        break;
    }

}

/******************************************************************************/
static void destroyPixbuf ( guchar *pixels, gpointer data ) {
    free ( pixels );
}

/******************************************************************************/
static void gtk3_m3duipatternpreview_createImage ( GTK3M3DUIPATTERNPREVIEW *gtk3patpreview ) {
    unsigned char (*data)[0x03] = calloc ( 0x01, gtk3patpreview->core.pat->size * 
                                                 gtk3patpreview->core.pat->size * 0x03 );
    int i, j;

    if ( gtk3patpreview->img ) g_object_unref ( gtk3patpreview->img );

    for ( i = 0x00; i < gtk3patpreview->core.pat->size; i++ ) {
        for ( j = 0x00; j < gtk3patpreview->core.pat->size; j++ ) {
            uint32_t offset = ( i * gtk3patpreview->core.pat->size ) + j;

            data[offset][0x00] = 0xFF - gtk3patpreview->core.pat->buffer[offset];
            data[offset][0x01] = 0xFF - gtk3patpreview->core.pat->buffer[offset];
            data[offset][0x02] = 0xFF - gtk3patpreview->core.pat->buffer[offset];
        }
    }

    /*** Convert to a GdkPixbuf image ***/
    gtk3patpreview->img = gdk_pixbuf_new_from_data ( (const guchar *) data,
                                                     GDK_COLORSPACE_RGB, 
                                                     FALSE,
                                                     0x08,
                                                     gtk3patpreview->core.pat->size,
                                                     gtk3patpreview->core.pat->size,
                                                     gtk3patpreview->core.pat->size * 0x03,
                                                     destroyPixbuf,
                                                     NULL );
}

/******************************************************************************/
static 
GTK3M3DUIPATTERNPREVIEW *gtk3_m3duipatternpreview_new ( M3DPATTERN  *pat,
                                                        uint32_t     width,
                                                        uint32_t     height ) {
    uint32_t structsize = sizeof ( GTK3M3DUIPATTERNPREVIEW );
    GTK3M3DUIPATTERNPREVIEW *gtk3patpreview = ( GTK3M3DUIPATTERNPREVIEW * ) calloc ( 0x01, structsize );

    if ( gtk3patpreview == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    gtk3patpreview->core.pat = pat;

    gtk3_m3duipatternpreview_createImage ( gtk3patpreview );


    return gtk3patpreview;
}

/******************************************************************************/
static void Size ( GtkWidget     *self,
                   GtkAllocation *allocation,
                   gpointer       user_data ) {
    GTK3M3DUIPATTERNLIST *gtk3patlist = ( GTK3M3DUIPATTERNLIST * ) user_data;

    m3duipatternlist_arrangePreviews ( &gtk3patlist->core,
                                        allocation->width,
                                        allocation->height );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3M3DUIPATTERNLIST *gtk3patlist = ( GTK3M3DUIPATTERNLIST * ) user_data;

    list_free ( &gtk3patlist->core.lpreview, gtk3_m3duipatternpreview_free );

    m3duipatternlist_clearPatterns ( &gtk3patlist->core );

    free ( gtk3patlist );
}

/******************************************************************************/
static void GeneratePreviews ( GTK3M3DUIPATTERNLIST *gtk3patlist ) {
    LIST *ltmppat = gtk3patlist->core.lpattern;

    while ( ltmppat ) {
        M3DPATTERN *pat = ( M3DPATTERN * ) ltmppat->data;

        GTK3M3DUIPATTERNPREVIEW *gtk3patpreview;

        gtk3patpreview = gtk3_m3duipatternpreview_new ( pat,
                                                        gtk3patlist->core.image_width,
                                                        gtk3patlist->core.image_height );

        list_insert ( &gtk3patlist->core.lpreview, gtk3patpreview );

        ltmppat = ltmppat->next;
    }
}

/******************************************************************************/
static GTK3M3DUIPATTERNLIST *gtk3_m3duipatternlist_new ( GTK3M3DUI *gtk3mui ) {
    GTK3M3DUIPATTERNLIST *gtk3patlist = calloc ( 0x01, sizeof ( GTK3M3DUIPATTERNLIST ) );

    if ( gtk3patlist == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3patlist->core.mui = ( M3DUI * ) gtk3mui;

    m3duipatternlist_init ( &gtk3patlist->core, 64 );


    return gtk3patlist; 
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3M3DUIPATTERNLIST *gtk3patlist = ( GTK3M3DUIPATTERNLIST * ) user_data;

    GeneratePreviews ( gtk3patlist );
}

/******************************************************************************/
GTK3M3DUIPATTERNLIST *gtk3_m3duipatternlist_create ( GtkWidget *parent,
                                                     GTK3M3DUI *gtk3mui,
                                                     char      *name ) {
    GTK3M3DUIPATTERNLIST *gtk3patlist = gtk3_m3duipatternlist_new ( gtk3mui );
    GtkWidget *scrolled = ui_gtk_scrolled_window_new ( CLASS_MAIN, NULL, NULL );
    GtkWidget *drw = ui_gtk_drawing_area_new ( CLASS_MAIN );

    gtk3patlist->scrolled = scrolled;
    gtk3patlist->area = drw;

    gtk_widget_set_size_request ( GTK_WIDGET(gtk3patlist->area), 300, 128 );

    gtk_widget_set_name ( scrolled, name );
    gtk_widget_set_name ( drw     , name );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_container_add ( GTK_CONTAINER(scrolled), drw );
#else
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(scrolled), drw );
#endif

    /*** For keyboard inputs ***/
    gtk_widget_set_can_focus ( drw, TRUE );

    /*** Drawing area does not receive mous events by defaults ***/
    gtk_widget_set_events ( drw, gtk_widget_get_events ( drw )  |
                                 GDK_KEY_PRESS_MASK             |
			                     GDK_KEY_RELEASE_MASK           |
                                 GDK_BUTTON_PRESS_MASK          |
                                 GDK_BUTTON_RELEASE_MASK        |
                                 GDK_POINTER_MOTION_MASK        |
                                 GDK_POINTER_MOTION_HINT_MASK );

    gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW(scrolled),
                                     GTK_POLICY_AUTOMATIC,
                                     GTK_POLICY_AUTOMATIC );

    g_signal_connect ( G_OBJECT (drw), "realize", G_CALLBACK (Realize), gtk3patlist );
    g_signal_connect ( G_OBJECT (drw), "destroy", G_CALLBACK (Destroy), gtk3patlist );
    g_signal_connect ( G_OBJECT (drw), "draw"   , G_CALLBACK (Draw)   , gtk3patlist );
    g_signal_connect ( G_OBJECT (drw), "size"   , G_CALLBACK (Size)   , gtk3patlist );

    g_signal_connect ( G_OBJECT (drw), "motion_notify_event" , G_CALLBACK (Input), gtk3patlist );
    g_signal_connect ( G_OBJECT (drw), "button-press-event"  , G_CALLBACK (Input), gtk3patlist );
    g_signal_connect ( G_OBJECT (drw), "button-release-event", G_CALLBACK (Input), gtk3patlist );
    g_signal_connect ( G_OBJECT (drw), "key_press_event"     , G_CALLBACK (Input), gtk3patlist );
    g_signal_connect ( G_OBJECT (drw), "key_release_event"   , G_CALLBACK (Input), gtk3patlist );

    gtk_widget_show ( drw );
    gtk_widget_show ( scrolled );

    return gtk3patlist;
}

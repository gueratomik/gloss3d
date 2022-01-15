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
static void Resize ( GtkWidget *, GdkRectangle *, gpointer );
static GTK3PATTERNPREVIEW *gtk3patternpreview_new ( M3DPATTERN  *pat );

/******************************************************************************/
static GTK3PATTERNPREVIEW *patternlistdata_pickPreview ( PATTERNLISTDATA *pdata,
                                                         uint32_t         x,
                                                         uint32_t         y ) {
    LIST *ltmppreview = pdata->lpreview;

    while ( ltmppreview ) {
        GTK3PATTERNPREVIEW *preview = ( GTK3PATTERNPREVIEW * ) ltmppreview->data;

        if ( ( x > preview->rec.x ) && ( x < ( preview->rec.x + 
                                               preview->rec.width  ) ) &&
             ( y > preview->rec.y ) && ( y < ( preview->rec.y + 
                                               preview->rec.height ) ) ) {
            return preview;
        }

        ltmppreview = ltmppreview->next;
    }

    return NULL;
}

/******************************************************************************/
static uint32_t patternlistdata_arrangePreviews ( PATTERNLISTDATA *pdata,
                                                  uint32_t         win_x,
                                                  uint32_t         win_y,
                                                  uint32_t         win_width,
                                                  uint32_t         win_height ) {
    LIST *ltmppreview = pdata->lpreview;
    uint32_t maxheight = 0x00;
    uint32_t nbpat = 0x00;

    while ( ltmppreview ) {
        GTK3PATTERNPREVIEW *preview = ( GTK3PATTERNPREVIEW * ) ltmppreview->data;

        if ( nbpat == 0x06 ) {
            win_y += pdata->preview_height;

            win_x  = 0x00;

            nbpat = 0x00;
        }

        preview->rec.x = win_x;
        preview->rec.y = win_y;

        preview->rec.width  = pdata->preview_width;
        preview->rec.height = pdata->preview_height;

        win_x += pdata->preview_width;

        if ( win_y + pdata->preview_height > maxheight ) {
            maxheight = win_y + pdata->preview_height;
        }

        nbpat++;

        ltmppreview = ltmppreview->next;
    }

    return maxheight;
}

/******************************************************************************/
static void patternlistdata_addPreview ( PATTERNLISTDATA *pdata, 
                                         M3DPATTERN      *pat ) {
    GTK3PATTERNPREVIEW *preview = gtk3patternpreview_new ( pat );

    list_insert ( &pdata->lpreview, preview );
}

/******************************************************************************/
void g3duipatternlist_input ( GtkWidget *widget, 
                              GdkEvent  *gdkev, 
                              gpointer   user_data ) {
    PATTERNLISTDATA *pdata = g_object_get_data ( G_OBJECT(widget),
                                                 GTK3WIDGETDATA );
    M3DUI *lui = ( M3DUI * ) user_data;
    G3DUI *gui = lui->gui;

    switch ( gdkev->type ) {
        case GDK_BUTTON_PRESS : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;
            GTK3PATTERNPREVIEW *preview;

            /*** For keyboard inputs ***/
            /*gtk_widget_grab_focus ( widget );*/

            preview = patternlistdata_pickPreview ( pdata, bev->x, bev->y );

            if ( preview ) {
                M3DSYSINFO *sysinfo = m3dsysinfo_get ( );

                /*** resize to the size of the pattern used until now ***/
                if ( sysinfo->pattern ) {
                    m3dpattern_resize ( preview->pat, sysinfo->pattern->size );
                }

                m3dsysinfo_setPattern ( sysinfo, preview->pat );
                /*lui->selpat = preview->pat; */

                m3dui_updateMouseToolEdit ( lui );
            }
        } break;

        default:
        break;
    }

}

/******************************************************************************/
void g3duipatternlist_drawPreview ( GtkWidget          *widget,
                                    cairo_t            *cr,
                                    GTK3PATTERNPREVIEW *preview,
                                    uint32_t            selected ) {
    PATTERNLISTDATA *pdata = g_object_get_data ( G_OBJECT(widget), 
                                                  GTK3WIDGETDATA );
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    GdkRGBA fg;

    gtk_style_context_get_color ( context, GTK_STATE_FLAG_NORMAL, &fg );

    gtk_render_frame ( context, cr, preview->rec.x,
                                    preview->rec.y, 
                                    preview->rec.width, 
                                    preview->rec.height );

    if ( selected ) {
        cairo_set_source_rgb ( cr, 0.0f, 0.0f, 1.0f );
        cairo_rectangle ( cr, preview->rec.x,
                              preview->rec.y,
                              preview->rec.width,
                              preview->rec.height );
        cairo_set_line_width ( cr, 1.0f );
        cairo_stroke ( cr );
    }

    /*** Draw the texture preview ***/
    gdk_cairo_set_source_pixbuf ( cr, 
                                  preview->img, 
                                  preview->rec.x + pdata->preview_border,
                                  preview->rec.y + pdata->preview_border );

    cairo_paint ( cr );
}

/******************************************************************************/
void gtk3patternpreview_free ( GTK3PATTERNPREVIEW *preview ) {
    g_object_unref ( preview->img );

    /*** TODO : free image data ***/


    free ( preview );
}

/******************************************************************************/
static void destroyPixbuf ( guchar *pixels, gpointer data ) {
    free ( pixels );
}

/******************************************************************************/
void gtk3patternpreview_create ( GTK3PATTERNPREVIEW *preview ) {
    unsigned char (*data)[0x03] = calloc ( 0x01, preview->pat->size * 
                                                 preview->pat->size * 0x03 );
    int i, j;

    if ( preview->img ) g_object_unref ( preview->img );

    for ( i = 0x00; i < preview->pat->size; i++ ) {
        for ( j = 0x00; j < preview->pat->size; j++ ) {
            uint32_t offset = ( i * preview->pat->size ) + j;

            data[offset][0x00] = 0xFF - preview->pat->buffer[offset];
            data[offset][0x01] = 0xFF - preview->pat->buffer[offset];
            data[offset][0x02] = 0xFF - preview->pat->buffer[offset];
        }
    }

    /*** Convert to a GdkPixbuf image ***/
    preview->img = gdk_pixbuf_new_from_data ( (const guchar *) data,
                                              GDK_COLORSPACE_RGB, 
                                              FALSE,
                                              0x08,
                                              preview->pat->size,
                                              preview->pat->size,
                                              preview->pat->size * 0x03,
                                              destroyPixbuf, NULL );
}

/******************************************************************************/
static GTK3PATTERNPREVIEW *gtk3patternpreview_new ( M3DPATTERN  *pat ) {
    void *memarea = calloc ( 0x01, sizeof ( GTK3PATTERNPREVIEW ) );
    GTK3PATTERNPREVIEW *preview = ( GTK3PATTERNPREVIEW * ) memarea;

    if ( preview == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    preview->pat = pat;

    gtk3patternpreview_create ( preview );


    return preview;
}

/******************************************************************************/
static void Draw ( GtkWidget *widget, cairo_t *cr, gpointer user_data ) {
    PATTERNLISTDATA *pdata = g_object_get_data ( G_OBJECT(widget),
                                                  GTK3WIDGETDATA );
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    LIST *ltmppreview = pdata->lpreview;
    M3DUI *lui = ( M3DUI * ) user_data;
    G3DUI    *gui = lui->gui;
    G3DSCENE *sce = gui->sce;
    GtkAllocation allocation;
    uint32_t x = 0x00, y = 0x00;
    uint32_t maxheight = 0x00;

    gtk_widget_get_allocation ( widget, &allocation );

    gtk_render_background ( context, cr,
                                     0x00,
                                     0x00,
                                     allocation.width,
                                     allocation.height );

    while ( ltmppreview ) {
        GTK3PATTERNPREVIEW *preview = ( GTK3PATTERNPREVIEW * ) ltmppreview->data;
        uint32_t selected = 0x00;

        if ( preview->pat == lui->selpat ) {
            selected = 0x01;
        }

        g3duipatternlist_drawPreview ( widget, cr, preview, selected );

        ltmppreview = ltmppreview->next;
    }
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    PATTERNLISTDATA *pdata = g_object_get_data ( G_OBJECT(widget),
                                                  GTK3WIDGETDATA );
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    M3DUI *lui = ( M3DUI * ) user_data;
    G3DUI    *gui = lui->gui;
    int i;

    /*** Render with button style ***/
    gtk_style_context_add_class ( context, GTK_STYLE_CLASS_BUTTON );
}

/******************************************************************************/
/****** ! g3duipatternlist_size() should not handle the third parameter ******/
/****** because this function is called from function that pass NULL for it ***/
static void Resize ( GtkWidget *widget, GdkRectangle *allocation,
                                        gpointer user_data ) {
    PATTERNLISTDATA *pdata = g_object_get_data ( G_OBJECT(widget),
                                                 GTK3WIDGETDATA );
    uint32_t maxheight;

    maxheight = patternlistdata_arrangePreviews ( pdata, 
                                                  allocation->x,
                                                  allocation->y,
                                                  allocation->width,
                                                  allocation->height );

    gtk_widget_set_size_request ( widget, allocation->width, maxheight );
}

/******************************************************************************/
GtkWidget *createPatternList ( GtkWidget        *parent, 
                               M3DUI *lui,
                               char             *name,
                               gint              x,
                               gint              y,
                               gint              width,
                               gint              height ) {
    PATTERNLISTDATA *pdata = common_patternlistdata_new ( 0x20 );
    GdkRectangle scrrec = { 0, 0, width, height };
    GdkRectangle drwrec = { 0, 0, 0x120, 0x120  };
    M3DSYSINFO *sysinfo = m3dsysinfo_get ( );
    GtkWidget *scr, *drw;
    int i;

    scr = gtk_scrolled_window_new ( NULL, NULL );

    gtk_widget_set_name ( scr, name );

    gtk_widget_set_size_request ( scr, scrrec.width, scrrec.height );

    gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW(scr),
                                     GTK_POLICY_AUTOMATIC,
                                     GTK_POLICY_AUTOMATIC );

    gtk_fixed_put ( GTK_FIXED(parent), scr, x, y );



    /*** Drawing area within the Scrolled Window ***/
    drw = gtk_drawing_area_new ( );

    /*** we use PATTERNLISTDATA structure to store  ***/
    /*** the data used by the drawing area widget.    ***/
    g_object_set_data ( G_OBJECT(drw), GTK3WIDGETDATA, pdata );

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

    /*** create patterns here ***/
    for ( i = 0x00; i < NBPATTERNS; i++ ) {
        patternlistdata_addPreview ( pdata,  pdata->patterns[i] );
    }

    m3dsysinfo_setPattern ( sysinfo, pdata->patterns[0x07] );

    g_signal_connect ( G_OBJECT (drw), "draw"      ,
                                       G_CALLBACK(Draw), lui );

    g_signal_connect ( G_OBJECT (drw), "realize"      ,
                                       G_CALLBACK(Realize), lui );

    g_signal_connect ( G_OBJECT (drw), "size-allocate",
                                       G_CALLBACK(Resize), lui );

    /*g_signal_connect ( G_OBJECT (drw), "motion_notify_event" , G_CALLBACK (materialista_input), gui );*/
    g_signal_connect ( G_OBJECT (drw), "button_press_event"  , G_CALLBACK (g3duipatternlist_input), lui );
    g_signal_connect ( G_OBJECT (drw), "button_release_event", G_CALLBACK (g3duipatternlist_input), lui );
    g_signal_connect ( G_OBJECT (drw), "key_press_event"     , G_CALLBACK (g3duipatternlist_input), lui );
    g_signal_connect ( G_OBJECT (drw), "key_release_event"   , G_CALLBACK (g3duipatternlist_input), lui );

    gtk_widget_set_size_request ( drw, drwrec.width, drwrec.height );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_container_add ( GTK_CONTAINER(scr), drw );
#else
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(scr), drw );
#endif

    gtk_widget_show ( drw );
    gtk_widget_show ( scr );


    return scr;
}

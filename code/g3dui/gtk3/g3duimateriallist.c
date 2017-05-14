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
static void Resize ( GtkWidget *, GdkRectangle *, gpointer );

/******************************************************************************/
static GTK3MATERIALPREVIEW *materiallistdata_pickPreview ( MATERIALLISTDATA *mdata,
                                                           uint32_t x,
                                                           uint32_t y ) {
    LIST *ltmppreview = mdata->lpreview;

    while ( ltmppreview ) {
        GTK3MATERIALPREVIEW *preview = ( GTK3MATERIALPREVIEW * ) ltmppreview->data;

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
static uint32_t materiallistdata_arrangePreviews ( MATERIALLISTDATA *mdata,
                                                   uint32_t win_x,
                                                   uint32_t win_y,
                                                   uint32_t win_width,
                                                   uint32_t win_height ) {
    LIST *ltmppreview = mdata->lpreview;
    uint32_t maxheight = 0x00;

    while ( ltmppreview ) {
        GTK3MATERIALPREVIEW *preview = ( GTK3MATERIALPREVIEW * ) ltmppreview->data;

        preview->rec.x = win_x;
        preview->rec.y = win_y;

        preview->rec.width  = mdata->preview_width;
        preview->rec.height = mdata->preview_height;

        win_x += mdata->preview_width;

        if ( win_y + mdata->preview_height > maxheight ) {
            maxheight = win_y + mdata->preview_height;
        }

        if ( ( win_width - win_x ) < mdata->preview_width ) {
            win_y += mdata->preview_height;

            win_x  = 0x00;
        }

        ltmppreview = ltmppreview->next;
    }

    return maxheight;
}

/******************************************************************************/
static GTK3MATERIALPREVIEW *materiallistdata_getPreview ( MATERIALLISTDATA *mdata,
                                                          G3DMATERIAL *mat ) {
    LIST *ltmppreview = mdata->lpreview;

    while ( ltmppreview ) {
        GTK3MATERIALPREVIEW *preview = ( GTK3MATERIALPREVIEW * ) ltmppreview->data;

        if ( preview->mat == mat ) return preview;

        ltmppreview = ltmppreview->next;
    }

    return NULL;
}

/******************************************************************************/
static void materiallistdata_removePreview ( MATERIALLISTDATA *mdata, 
                                             G3DMATERIAL *mat ) {
    GTK3MATERIALPREVIEW *preview = materiallistdata_getPreview ( mdata, mat );

    list_remove ( &mdata->lpreview, preview );
}

/******************************************************************************/
static void materiallistdata_addPreview ( MATERIALLISTDATA *mdata, 
                                          G3DMATERIAL *mat ) {
    GTK3MATERIALPREVIEW *preview = gtk3materialpreview_new ( mat, mdata->matmap );

    list_insert ( &mdata->lpreview, preview );
}

/******************************************************************************/
static void materiallistdata_removeAllPreviews ( MATERIALLISTDATA *mdata ) {
    list_free ( &mdata->lpreview, (void(*)(void*)) gtk3materialpreview_free );
}

/******************************************************************************/
void g3duimateriallist_input ( GtkWidget *widget, GdkEvent *gdkev, 
                                                  gpointer user_data ) {
    MATERIALLISTDATA *mdata = g_object_get_data ( G_OBJECT(widget),
                                                  GTK3WIDGETDATA );
    G3DUI            *gui   = ( G3DUI * ) user_data;

    switch ( gdkev->type ) {
        case GDK_KEY_PRESS : {
            GdkEventKey *kev = ( GdkEventKey * ) gdkev;

            switch ( kev->keyval ) {
                case GDK_KEY_Delete: {
                    if ( gui->selmat ) {
                        g3duimateriallist_removeMaterial ( widget, gui->sce, 
                                                                   gui->urm,
                                                                   gui->selmat );

                        gui->selmat = NULL;
                    }
                } break;
            }

            g3dui_redrawMaterialList ( gui );
            g3dui_updateMaterialEdit ( gui );
            g3dui_redrawGLViews      ( gui );
        } break;

        case GDK_BUTTON_PRESS : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;
            GTK3MATERIALPREVIEW *preview;

            /*** For keyboard inputs ***/
            gtk_widget_grab_focus ( widget );

            preview = materiallistdata_pickPreview ( mdata, bev->x, bev->y );

            if ( preview ) gui->selmat = preview->mat;

            g3dui_redrawMaterialList ( gui );
            g3dui_updateMaterialEdit ( gui );
            g3dui_redrawGLViews      ( gui );
        } break;

        default:
        break;
    }

}

/******************************************************************************/
void g3duimateriallist_importFromScene ( GtkWidget *widget, G3DSCENE *sce ) {
    LIST *ltmpmat = sce->lmat;

    while ( ltmpmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpmat->data;

        g3duimateriallist_addPreview ( widget, mat );

        ltmpmat = ltmpmat->next;
    }
}

/******************************************************************************/
void g3duimateriallist_removeAllPreviews ( GtkWidget *widget ) {
    MATERIALLISTDATA *mdata = g_object_get_data ( G_OBJECT(widget),
                                                  GTK3WIDGETDATA );

    materiallistdata_removeAllPreviews ( mdata );
}

/******************************************************************************/
void g3duimateriallist_updatePreview ( GtkWidget *widget, G3DMATERIAL *mat ) {
    MATERIALLISTDATA *mdata = g_object_get_data ( G_OBJECT(widget),
                                                  GTK3WIDGETDATA );
    GTK3MATERIALPREVIEW *preview = materiallistdata_getPreview ( mdata, mat );

    if ( preview ) gtk3materialpreview_update ( preview, mdata->matmap );

    gtk_widget_queue_draw_area ( widget, preview->rec.x, 
                                         preview->rec.y,
                                         preview->rec.width, 
                                         preview->rec.height );
}

/******************************************************************************/
void g3duimateriallist_addPreview ( GtkWidget *widget, G3DMATERIAL *mat ) {
    MATERIALLISTDATA *mdata = g_object_get_data ( G_OBJECT(widget),
                                                  GTK3WIDGETDATA );
    GtkAllocation allocation;

    gtk_widget_get_allocation ( widget, &allocation );

    materiallistdata_addPreview ( mdata, mat );

    /*** ! g3duimateriallist_size() should not handle the third parameter ***/
    Resize ( widget, &allocation, NULL );
}

/******************************************************************************/
void g3duimateriallist_removeMaterial ( GtkWidget *widget, G3DSCENE *sce,
                                                           G3DURMANAGER *urm,
                                                           G3DMATERIAL *mat ) {
    g3dscene_delMaterial ( sce, mat );

    g3duimateriallist_removePreview ( widget, mat );
}

/******************************************************************************/
void g3duimateriallist_removePreview ( GtkWidget *widget, G3DMATERIAL *mat ) {
    MATERIALLISTDATA    *mdata  = g_object_get_data ( G_OBJECT(widget),
                                                      GTK3WIDGETDATA );
    GtkAllocation allocation;

    gtk_widget_get_allocation ( widget, &allocation );

    materiallistdata_removePreview ( mdata, mat );

    /*** ! g3duimateriallist_size() should not handle the third parameter ***/
    Resize ( widget, &allocation, NULL );
}

/******************************************************************************/
void g3duimateriallist_drawPreview ( GtkWidget *widget,
                                     cairo_t *cr,
                                     GTK3MATERIALPREVIEW *preview,
                                     uint32_t selected ) {
    MATERIALLISTDATA *mdata = g_object_get_data ( G_OBJECT(widget), 
                                                  GTK3WIDGETDATA );
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    cairo_text_extents_t te;
    float text_x, text_y;
    GdkRGBA fg;

    /*** Draw the texture name ***/
    cairo_text_extents ( cr, preview->mat->name, &te );

    gtk_style_context_get_color ( context, GTK_STATE_FLAG_NORMAL, &fg );

    text_x = preview->rec.x + ( preview->rec.width  - te.width ) * 0.5f;
    text_y = preview->rec.y + ( preview->rec.height - mdata->preview_name_height / 2 );

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

    cairo_set_source_rgba ( cr, fg.red, fg.green, fg.blue, fg.alpha );
    cairo_move_to        ( cr, text_x, text_y );
    cairo_show_text      ( cr, preview->mat->name );


    /*** Draw the texture preview ***/
    gdk_cairo_set_source_pixbuf ( cr, preview->img, preview->rec.x + mdata->preview_border,
                                                    preview->rec.y + mdata->preview_border );

    cairo_paint ( cr );
}

/******************************************************************************/
void gtk3materialpreview_free ( GTK3MATERIALPREVIEW *preview ) {
    g_object_unref ( preview->img );

    /*** TODO : free image data ***/


    free ( preview );
}

/******************************************************************************/
static void destroyPixbuf ( guchar *pixels, gpointer data ) {
    free ( pixels );
}

/******************************************************************************/
void gtk3materialpreview_update ( GTK3MATERIALPREVIEW *preview,
                                  G3DUIMATERIALMAP    *matmap ) {
    unsigned char (*data)[0x03];

    if ( preview->img ) g_object_unref ( preview->img );

    preview->img = NULL;

    /*** Alloc a buffer that is filled with the material preview ***/
    data = calloc ( 0x03, matmap->width * matmap->height );

    if ( data == NULL ) {
        fprintf ( stderr, "gtk3materialpreview_new: memory allocation failed\n" );

        free ( preview );
    }

    common_g3duimaterialmap_fillData ( matmap, preview->mat, data );

    /*** Convert to a GdkPixbuf image ***/
    preview->img = gdk_pixbuf_new_from_data ( (const guchar *) data,
                                              GDK_COLORSPACE_RGB, 
                                              FALSE,
                                              0x08,
                                              matmap->width,
                                              matmap->height,
                                              matmap->width * 0x03,
                                              destroyPixbuf, NULL );
}

/******************************************************************************/
GTK3MATERIALPREVIEW *gtk3materialpreview_new ( G3DMATERIAL *mat, 
                                               G3DUIMATERIALMAP *matmap ) {
    uint32_t structsize = sizeof ( GTK3MATERIALPREVIEW );
    GTK3MATERIALPREVIEW *preview = ( GTK3MATERIALPREVIEW * ) calloc ( 0x01, structsize );


    if ( preview == NULL ) {
        fprintf ( stderr, "gtk3materialpreview_new: memory allocation failed\n" );

        return NULL;
    }

    preview->mat = mat;

    gtk3materialpreview_update ( preview, matmap );


    return preview;
}

/******************************************************************************/
static void Draw ( GtkWidget *widget, cairo_t *cr, gpointer user_data ) {
    MATERIALLISTDATA *matlst = g_object_get_data ( G_OBJECT(widget),
                                                   GTK3WIDGETDATA );
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    LIST *ltmppreview = matlst->lpreview;
    G3DUI    *gui = ( G3DUI * ) user_data;
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
        GTK3MATERIALPREVIEW *preview = ( GTK3MATERIALPREVIEW * ) ltmppreview->data;
        uint32_t selected = 0x00;

        if ( preview->mat == gui->selmat ) {
            selected = 0x01;
        }

        g3duimateriallist_drawPreview ( widget, cr, preview, selected );

        ltmppreview = ltmppreview->next;
    }
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );

    /*** Render with button style ***/
    gtk_style_context_add_class ( context, GTK_STYLE_CLASS_BUTTON );
}

/******************************************************************************/
/****** ! g3duimateriallist_size() should not handle the third parameter ******/
/****** because this function is called from function that pass NULL for it ***/
static void Resize ( GtkWidget *widget, GdkRectangle *allocation,
                                        gpointer user_data ) {
    MATERIALLISTDATA *mdata = g_object_get_data ( G_OBJECT(widget),
                                                  GTK3WIDGETDATA );
    uint32_t maxheight;

    maxheight = materiallistdata_arrangePreviews ( mdata, allocation->x,
                                                          allocation->y,
                                                          allocation->width,
                                                          allocation->height );

    gtk_widget_set_size_request ( widget, allocation->width, maxheight );
}

/******************************************************************************/
GtkWidget *createMaterialList ( GtkWidget *parent, G3DUI *gui,
                                                    char *name,
                                                    gint x,
                                                    gint y,
                                                    gint width,
                                                    gint height ) {
    MATERIALLISTDATA *matlst = common_materiallistdata_new ( 0x60, 0x60 );
    GdkRectangle scrrec = { 0, 0, width, height };
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

    /*** we use MATERIALLISTDATA structure to store  ***/
    /*** the data used by the drawing area widget.    ***/
    g_object_set_data ( G_OBJECT(drw), GTK3WIDGETDATA, matlst );

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

    g_signal_connect ( G_OBJECT (drw), "draw"      ,
                                       G_CALLBACK(Draw), gui );

    g_signal_connect ( G_OBJECT (drw), "realize"      ,
                                       G_CALLBACK(Realize), gui );

    g_signal_connect ( G_OBJECT (drw), "size-allocate",
                                       G_CALLBACK(Resize), gui );

    /*g_signal_connect ( G_OBJECT (drw), "motion_notify_event" , G_CALLBACK (materialista_input), gui );*/
    g_signal_connect ( G_OBJECT (drw), "button_press_event"  , G_CALLBACK (g3duimateriallist_input), gui );
    g_signal_connect ( G_OBJECT (drw), "button_release_event", G_CALLBACK (g3duimateriallist_input), gui );
    g_signal_connect ( G_OBJECT (drw), "key_press_event"     , G_CALLBACK (g3duimateriallist_input), gui );
    g_signal_connect ( G_OBJECT (drw), "key_release_event"   , G_CALLBACK (g3duimateriallist_input), gui );

    gtk_widget_set_size_request ( drw, drwrec.width, drwrec.height );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_container_add ( GTK_CONTAINER(scr), drw );
#else
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(scr), drw );
#endif

    list_insert ( &gui->lmatlist, drw );

    gtk_widget_show ( drw );
    gtk_widget_show ( scr );


    return scr;
}

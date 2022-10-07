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
GTK3G3DUIMATERIALPREVIEW *gtk3_g3duimaterialpreview_new ( G3DMATERIAL *mat,
                                                          uint32_t     width,
                                                          uint32_t     height ) {
    uint32_t structsize = sizeof ( GTK3G3DUIMATERIALPREVIEW );
    GTK3G3DUIMATERIALPREVIEW *gtk3matpreview = ( GTK3G3DUIMATERIALPREVIEW * ) calloc ( 0x01, structsize );

    if ( gtk3matpreview == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    gtk3matpreview->core.mat = mat;
    /* TODO: should be the same map for all */
    gtk3matpreview->core.map = g3duimaterialmap_new ( width, height );

    gtk3_g3duimaterialpreview_update ( gtk3matpreview );


    return gtk3matpreview;
}

/******************************************************************************/
void gtk3_g3duimaterialpreview_free ( GTK3G3DUIMATERIALPREVIEW *gtk3preview ) {
    g_object_unref ( gtk3preview->img );

    /*** TODO : free image data ***/

    free ( gtk3preview );
}

/******************************************************************************/
void gtk3_g3duimaterialpreview_update ( GTK3G3DUIMATERIALPREVIEW *gtk3matpreview ) {
    unsigned char (*data)[0x03];

    /*** build the material sphere vector map ***/
    g3duimaterialmap_buildSphere ( gtk3matpreview->core.map,
                                   gtk3matpreview->core.mat, 0.8f );

    if ( gtk3matpreview->img ) g_object_unref ( gtk3matpreview->img );

    gtk3matpreview->img = NULL;

    /*** Alloc a buffer that is filled with the material preview ***/
    data = calloc ( 0x03, gtk3matpreview->core.map->width * 
                          gtk3matpreview->core.map->height );

    if ( data == NULL ) {
        fprintf ( stderr, "gtk3materialpreview_new: memory allocation failed\n" );
    }

    g3duimaterialmap_fillData ( preview->map, 
                                preview->mat, data );

    /*** Convert to a GdkPixbuf image ***/
    gtk3matpreview->img = gdk_pixbuf_new_from_data ( (const guchar *) data,
                                                     GDK_COLORSPACE_RGB, 
                                                     FALSE,
                                                     0x08,
                                                     gtk3matpreview->core.map->width,
                                                     gtk3matpreview->core.map->height,
                                                     gtk3matpreview->core.map->width * 0x03,
                                                     destroyPixbuf,
                                                     NULL );
}

/******************************************************************************/
static void gtk3_g3duimateriallist_removeAllMaterials ( GTK3G3DUIMATERIALLIST *gtk3matlist ) {
    list_free ( &gtk3matlist->core.lpreview, (void(*)(void*)) gtk3_g3duimaterialpreview_free );
}

/******************************************************************************/
static GTK3G3DUIMATERIALLIST *gtk3_g3duimateriallist_new ( GTK3G3DUI *gtk3gui,
                                                           uint32_t   width,
                                                           uint32_t   height ) {
    GTK3G3DUIMATERIALLIST *gtk3matlist = calloc ( 0x01, sizeof ( GTK3G3DUIMATERIALLIST ) );

    if ( gtk3matlist == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    g3duimateriallist_init ( &gtk3matlist->core, &gtk3gui->core, width, height );


    return gtk3matlist; 
}

/******************************************************************************/
static void gtk3_g3duimateriallist_free ( GTK3G3DUIMATERIALLIST *gtk3matlist ) {
    gtk3_g3duimateriallist_removeAllMaterials ( gtk3matlist );

    free ( gtk3matlist );
}

/******************************************************************************/
static void gtk3_g3duimateriallist_updatePreview ( GTK3G3DUIMATERIALLIST *gtk3matlist,
                                                   G3DMATERIAL           *mat ) {
    G3DUIMATERIALLIST *matlist = ( G3DUIMATERIALLIST * ) &gtk3matlist->core;
    GTK3G3DUIMATERIALPREVIEW *gtk3preview = g3duimateriallist_getPreview ( matlist,
                                                                           mat );

    if ( preview ) gtk3_g3duimaterialpreview_new ( gtk3preview );

    gtk_widget_queue_draw_area ( widget,
                                 preview->rec.x, 
                                 preview->rec.y,
                                 preview->rec.width, 
                                 preview->rec.height );
}

/******************************************************************************/
void gtk3_g3duimateriallist_addMaterial ( GTK3G3DUIMATERIALLIST *gtk3matlist,
                                          G3DMATERIAL           *mat ) {
    G3DUIMATERIALLIST *matlist = ( G3DUIMATERIALLIST * ) &gtk3matlist->core;
    GTK3G3DUIMATERIALPREVIEW *gtk3preview;
    GtkAllocation allocation;

    gtk_widget_get_allocation ( gtk3matlist->area, &allocation );

    gtk3preview = gtk3_g3duimaterialpreview_new ( mat, 
                                                  allocation.width,
                                                  allocation.height );

    g3duimateriallist_addPreview ( matlist, gtk3preview );

    /* TODO: redraw */
}

/******************************************************************************/
void gtk3_g3duimateriallist_removeMaterial ( GTK3G3DUIMATERIALLIST *gtk3matlist, 
                                             G3DSCENE              *sce,
                                             G3DURMANAGER          *urm,
                                             G3DMATERIAL           *mat ) {
    g3durm_scene_removeMaterial ( urm, 
                                  sce, 
                                  mat, 
                                  0x00,
                                  REDRAWLIST | REBUILDMATERIALLIST  );

    g3duimateriallist_removePreview ( &gtk3matlist->core, mat );

    /* TODO: redraw */
}

/******************************************************************************/
void gtk3_g3duimateriallist_importFromScene ( GTK3G3DUIMATERIALLIST *gtk3matlist,
                                              G3DSCENE              *sce ) {
    LIST *ltmpmat = sce->lmat;

    while ( ltmpmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpmat->data;

        gtk3_g3duimateriallist_addMaterial ( gtk3matlist, mat );

        ltmpmat = ltmpmat->next;
    }
}

/******************************************************************************/
void gtk3_g3duimateriallist_drawPreview ( GTK3G3DUIMATERIALLIST    *gtk3matlist,
                                          cairo_t                  *cr,
                                          GTK3G3DUIMATERIALPREVIEW *gtk3preview,
                                          uint32_t                  selected ) {
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    cairo_text_extents_t te;
    float text_x, text_y;
    GdkRGBA fg;

    /*** Draw the texture name ***/
    cairo_text_extents ( cr, preview->mat->name, &te );

    gtk_style_context_get_color ( context, GTK_STATE_FLAG_NORMAL, &fg );

    text_x = gtk3preview->core.rec.x + ( gtk3preview->core.rec.width  - te.width ) * 0.5f;
    text_y = gtk3preview->core.rec.y + ( gtk3preview->core.rec.height - gtk3matlist->core.preview_name_height / 2 );

    gtk_render_frame ( context,
                       cr,
                       gtk3preview->core.rec.x,
                       gtk3preview->core.rec.y, 
                       gtk3preview->core.rec.width, 
                       gtk3preview->core.rec.height );

    if ( selected ) {
        cairo_set_source_rgb ( cr, 0.0f, 0.0f, 1.0f );

        cairo_rectangle ( cr,
                          gtk3preview->core.rec.x,
                          gtk3preview->core.rec.y,
                          gtk3preview->core.rec.width,
                          gtk3preview->core.rec.height );

        cairo_set_line_width ( cr, 1.0f );
        cairo_stroke ( cr );
    }

    cairo_set_source_rgba ( cr, fg.red, fg.green, fg.blue, fg.alpha );
    cairo_move_to        ( cr, text_x, text_y );
    cairo_show_text      ( cr, preview->mat->name );

    /*** Draw the texture preview ***/
    gdk_cairo_set_source_pixbuf ( cr,
                                  gtk3preview->img,
                                  gtk3preview->core.rec.x + gtk3matlist->core.preview_border,
                                  gtk3preview->core.rec.y + gtk3matlist->core.preview_border );

    cairo_paint ( cr );
}

/******************************************************************************/
static void destroyPixbuf ( guchar *pixels, gpointer data ) {
    free ( pixels );
}


/******************************************************************************/
void gtk3_g3duimateriallist_input ( GtkWidget *widget,
                                    GdkEvent  *gdkev, 
                                    gpointer   user_data ) {
    GTK3G3DUIMATERIALLIST *gtk3matlist = ( GTK3G3DUIMATERIALLIST * ) user_data;
    G3DUI *gui = ( G3DUI * ) gtk3matlist->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    switch ( gdkev->type ) {
        case GDK_KEY_PRESS : {
            GdkEventKey *kev = ( GdkEventKey * ) gdkev;

            switch ( kev->keyval ) {
                case GDK_KEY_Delete: {
                    if ( gui->selmat ) {
                        g3duimateriallist_removeMaterial ( &gtk3matlist->core,
                                                            gui->sce, 
                                                            gui->urm,
                                                            gui->selmat );

                        gui->selmat = NULL;
                    }
                } break;
            }

            gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW          |
                                                   REDRAWMATERIALLIST  | 
                                                   REDRAWCURRENTMATERIAL );
        } break;

        case GDK_BUTTON_PRESS : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;
            G3DUIMATERIALPREVIEW *preview;

            /*** For keyboard inputs ***/
            gtk_widget_grab_focus ( widget );

            preview = g3duimateriallist_pickPreview ( &gtk3matlist->core, 
                                                       bev->x,
                                                       bev->y );

            if ( preview ) gui->selmat = preview->mat;

            gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW          |
                                                   REDRAWMATERIALLIST  | 
                                                   REDRAWCURRENTMATERIAL );
        } break;

        default:
        break;
    }

}

/******************************************************************************/
static void Draw ( GtkWidget *widget,
                   cairo_t   *cr,
                   gpointer   user_data ) {
    GTK3G3DUIMATERIALLIST *gtk3matlist = ( GTK3G3DUIMATERIALLIST * ) user_data;
    G3DUI *gui = ( G3DUI * ) gtk3matlist->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    LIST *ltmppreview = gtk3matlist->core.lpreview;
    G3DSCENE *sce = gui->sce;
    GtkAllocation allocation;
    uint32_t x = 0x00, y = 0x00;
    uint32_t maxheight = 0x00;

    gtk_widget_get_allocation ( widget, &allocation );

    gtk_render_background ( context,
                            cr,
                            0x00,
                            0x00,
                            allocation.width,
                            allocation.height );

    while ( ltmppreview ) {
        GTK3G3DUIMATERIALPREVIEW *gtk3preview = ( GTK3G3DUIMATERIALPREVIEW * ) ltmppreview->data;
        uint32_t selected = 0x00;

        if ( gtk3preview->core.mat == gui->selmat ) {
            selected = 0x01;
        }

        g3duimateriallist_drawPreview ( gtk3matlist, cr, gtk3preview, selected );

        ltmppreview = ltmppreview->next;
    }
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMATERIALLIST *gtk3matlist = ( GTK3G3DUIMATERIALLIST * ) user_data;

    gtk3_g3duimateriallist_free ( gtk3matlist );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIMATERIALLIST *gtk3matlist = ( GTK3G3DUIMATERIALLIST * ) user_data;
    G3DUI *gui = ( G3DUI * ) gtk3matlist->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );

    /*** Render with button style ***/
    gtk_style_context_add_class ( context, GTK_STYLE_CLASS_BUTTON );

    gtk3_g3duimateriallist_removeAllMaterials ( gtk3matlist );
    gtk3_g3duimateriallist_importFromScene    ( gtk3matlist, gui->sce );
}

/******************************************************************************/
static void Resize ( GtkWidget    *widget,
                     GdkRectangle *allocation,
                     gpointer      user_data ) {
    GTK3G3DUIMATERIALLIST *gtk3matlist = ( GTK3G3DUIMATERIALLIST * ) user_data;
    G3DUI *gui = ( G3DUI * ) gtk3matlist->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    uint32_t maxsize, maxheight, maxwidth;

    maxsize = g3duimateriallist_arrangePreviews ( &gtk3matlist->core,
                                                   allocation->x,
                                                   allocation->y,
                                                   allocation->width,
                                                   allocation->height );

    maxheight = ( maxsize & 0xFFFF0000 ) >> 0x10;
    maxwidth  = ( maxsize & 0x0000FFFF );

    gtk_widget_set_size_request ( widget, maxwidth, maxheight );
}

/******************************************************************************/
GTK3G3DUIMATERIALLIST *gtk3_g3duimateriallist_create ( GtkWidget *parent,
                                                       GTK3G3DUI *gtk3gui,
                                                       char      *name ) {
    GTK3G3DUIMATERIALLIST *gtk3matlist = gtk3_g3duimateriallist_new ( gtk3gui, 
                                                                      0x60,
                                                                      0x60 );
    GtkWidget *scrolled = ui_gtk_scrolled_window_new ( CLASS_MAIN, NULL, NULL );
    GtkWidget *drw = ui_gtk_drawing_area_new ( CLASS_MAIN );

    gtk3matlist->scrolled = scrolled;
    gtk3matlist->area = drw;

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

    gtk_scrolled_window_set_policy ( GTK_SCROLLED_WINDOW(scr),
                                     GTK_POLICY_AUTOMATIC,
                                     GTK_POLICY_AUTOMATIC );

    g_signal_connect ( G_OBJECT (drw), "realize", G_CALLBACK (Realize), gtk3matlist );
    g_signal_connect ( G_OBJECT (drw), "destroy", G_CALLBACK (Destroy), gtk3matlist );
    g_signal_connect ( G_OBJECT (drw), "draw"   , G_CALLBACK (Draw)   , gtk3matlist );

    g_signal_connect ( G_OBJECT (drw), "motion_notify_event" , G_CALLBACK (Input), gtk3matlist );
    g_signal_connect ( G_OBJECT (drw), "button-press-event"  , G_CALLBACK (Input), gtk3matlist );
    g_signal_connect ( G_OBJECT (drw), "button-release-event", G_CALLBACK (Input), gtk3matlist );
    g_signal_connect ( G_OBJECT (drw), "key_press_event"     , G_CALLBACK (Input), gtk3matlist );
    g_signal_connect ( G_OBJECT (drw), "key_release_event"   , G_CALLBACK (Input), gtk3matlist );

    gtk_widget_show ( drw );
    gtk_widget_show ( scrolled );

    return scrolled;
}

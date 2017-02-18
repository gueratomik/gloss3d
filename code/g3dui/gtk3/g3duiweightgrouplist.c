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

/******************************************************************************/
#include <xpm/horz.xpm>
#include <xpm/vert.xpm>
#include <xpm/shor.xpm>
#include <xpm/bonico.xpm>
#include <xpm/ligico.xpm>
#include <xpm/mesico.xpm>
#include <xpm/defico.xpm>
#include <xpm/symico.xpm>
#include <xpm/cubico.xpm>
#include <xpm/sphico.xpm>
#include <xpm/torico.xpm>
#include <xpm/cylico.xpm>
#include <xpm/camico.xpm>
#include <xpm/scnico.xpm>
#include <xpm/ffdico.xpm>
#include <xpm/mapico.xpm>
#include <xpm/plnico.xpm>
#include <xpm/eyeopn.xpm>
#include <xpm/eyesht.xpm>
#include <xpm/enaico.xpm>
#include <xpm/disico.xpm>

/******************************************************************************/
static uint32_t drawMeshWeightGroups ( GtkStyleContext *context, cairo_t *cr,
                                                                 G3DMESH *mes ) {
    LIST *ltmpgrp = mes->lgrp;
    cairo_text_extents_t te;
    GdkRGBA fg;
    uint32_t maxheight = 0x00, maxwidth = 0x00;
    float grpy = 0;
    float y = 0;

    while ( ltmpgrp ) {
        G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmpgrp->data;

        cairo_text_extents ( cr, grp->name, &te );

        if ( maxwidth < te.width ) maxwidth = te.width;

        gtk_style_context_get_color ( context, GTK_STATE_FLAG_NORMAL, &fg );

        if ( grp->flags & WEIGHTGROUPSELECTED ) {
            cairo_set_source_rgba ( cr, 1.0f  , 0.0f    , 0.0f   , fg.alpha );
        } else {
            cairo_set_source_rgba ( cr, fg.red, fg.green, fg.blue, fg.alpha );
        }

        grpy = y - ( te.height / 2 + te.y_bearing ) + LISTINDENT * 0.5f;

        cairo_move_to   ( cr, 0, grpy );
        cairo_show_text ( cr, grp->name );

        y += LISTINDENT;


        ltmpgrp = ltmpgrp->next;
    }

    maxheight = ( y );


    return ( ( maxheight << 0x10 ) | maxwidth );
}

/******************************************************************************/
static void Input ( GtkWidget *widget, GdkEvent *gdkev, gpointer user_data ) {
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    G3DUI    *gui = ( G3DUI * ) user_data;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    G3DURMANAGER *urm = gui->urm;

    switch ( gdkev->type ) {
        case GDK_KEY_PRESS : {
            GdkEventKey *kev = ( GdkEventKey * ) gdkev;

            switch ( kev->keyval ) {
                case GDK_KEY_Delete: {
                    common_g3duiweightgrouplist_deleteSelectedCbk ( gui );
                } break;
            }

            gtk_widget_queue_draw ( widget );
        } break;

        case GDK_BUTTON_PRESS : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;

            /*** For keyboard inputs ***/
            gtk_widget_grab_focus ( widget );

            if ( obj && ( obj->type == G3DMESHTYPE ) ) {
                G3DMESH *mes= ( G3DMESH * ) obj;
                G3DWEIGHTGROUP *grp = common_g3duiweightgrouplist_getWeightGroup ( mes, bev->x,
                                                                                        bev->y );

                if ( grp ) {
                    common_g3duiweightgrouplist_selectCbk ( gui, grp );
                }
            }

            gtk_widget_queue_draw ( widget );
        } break;

        case GDK_MOTION_NOTIFY : {
            GdkEventMotion *mev = ( GdkEventMotion * ) gdkev;

        } break;

        case GDK_BUTTON_RELEASE : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;

        } break;

        default:
        break;
    }
}

/******************************************************************************/
static void Draw ( GtkWidget *widget, cairo_t *cr, gpointer user_data ) {
    G3DUI    *gui = ( G3DUI * ) user_data;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    GtkAllocation allocation;
    cairo_text_extents_t te;
    uint32_t width, height;
    uint32_t size = 0;
    float y = 0;

    gtk_widget_get_allocation ( widget, &allocation );

    gtk_render_background ( context, cr,
                                     0x00,
                                     0x00,
                                     allocation.width,
                                     allocation.height );

    if ( obj && obj->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        size = drawMeshWeightGroups ( context, cr, mes );
    }

    /*** Adjust drawing area size. This implies draw() is called ***/
    /*** twice. We might want to change that in the future.      ***/
    gtk_widget_set_size_request ( widget, size & 0x0000FFFF, size >> 0x10 );
}

/******************************************************************************/
GtkWidget *createWeightgroupList ( GtkWidget *parent, G3DUI *gui,
                                                      char *name,
                                                      gint x,
                                                      gint y,
                                                      gint width,
                                                      gint height ) {
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

/*    g_signal_connect ( G_OBJECT (drw), "size-allocate"       , G_CALLBACK (gtk3_sizeGL ), view );
    g_signal_connect ( G_OBJECT (drw), "realize"             , G_CALLBACK (gtk3_initGL ), view );
*/    
    g_signal_connect ( G_OBJECT (drw), "draw"                , G_CALLBACK (Draw ), gui );

    /*g_signal_connect ( G_OBJECT (drw), "motion_notify_event" , G_CALLBACK (weightgrouplistarea_input), gui );*/
    g_signal_connect ( G_OBJECT (drw), "button_press_event"  , G_CALLBACK (Input), gui );
    g_signal_connect ( G_OBJECT (drw), "button_release_event", G_CALLBACK (Input), gui );
    g_signal_connect ( G_OBJECT (drw), "key_press_event"     , G_CALLBACK (Input), gui );
    g_signal_connect ( G_OBJECT (drw), "key_release_event"   , G_CALLBACK (Input), gui );

    gtk_widget_set_size_request ( drw, drwrec.width, drwrec.height );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_container_add ( GTK_CONTAINER(scr), drw );
#else
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(scr), drw );
#endif

    list_insert ( &gui->lweightgroup, drw );

    gtk_widget_show ( drw );
    gtk_widget_show ( scr );


    return scr;
}

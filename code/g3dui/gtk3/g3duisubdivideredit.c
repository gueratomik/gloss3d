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

static void updateSubdivisionForm ( GtkWidget *, G3DUI * );

/******************************************************************************/
static void subdivSyncCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duisubdivideredit_subdivSyncCbk ( gui );
}

/******************************************************************************/
static void useIsoLinesCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    /*common_g3duimeshedit_useIsoLinesCbk ( gui );*/
}

/******************************************************************************/
static void subdivRenderCbk  ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent     ( widget );
    int level  = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( level >= 0x00 ) {
        common_g3duisubdivideredit_subdivRenderCbk ( gui, level );
    }

    updateSubdivisionForm ( parent, gui );
}

/******************************************************************************/
static void subdivPreviewCbk  ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int level  = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui    = ( G3DUI * ) user_data;

    if ( level >= 0x00 ) {
        common_g3duisubdivideredit_subdivPreviewCbk ( gui, level );
    }

    updateSubdivisionForm ( parent, gui );
}

/******************************************************************************/
static void updateSubdivisionForm ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DSUBDIVIDERTYPE ) ) {
        G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) obj;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( GTK_IS_SPIN_BUTTON(child) ) {
                GtkSpinButton *sbn = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITSUBDIVIDERPREVIEW ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, sdr->subdiv_preview );
                }

                if ( strcmp ( child_name, EDITSUBDIVIDERRENDER ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, sdr->subdiv_render );
                }
            }

            if ( GTK_IS_CHECK_BUTTON(child) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITMESHISOLINES ) == 0x00 ) {
                    if ( ((G3DOBJECT*)sdr)->flags & MESHUSEISOLINES ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }

                if ( strcmp ( child_name, EDITSUBDIVIDERSYNC ) == 0x00 ) {
                    if ( ((G3DOBJECT*)sdr)->flags & SYNCLEVELS ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
void updateSubdivisionFrame ( GtkWidget *widget, G3DUI *gui ) {
    GtkWidget *frm = gtk_bin_get_child ( GTK_BIN(widget) );

    if ( frm ) updateSubdivisionForm ( frm, gui );
}


/******************************************************************************/
static void createSubdivisionFrame ( GtkWidget *frm, G3DUI *gui,
                                                     gint x,
                                                     gint y,
                                                     gint width,
                                                     gint height ) {
    GtkWidget *sdf;

    sdf = createFrame ( frm, gui, EDITSUBDIVIDER       , x,   y, width, height );

    createIntegerText ( sdf, gui, EDITSUBDIVIDERPREVIEW, 0, 12,
                                                         0,   0, 128, 32,
                                                       subdivPreviewCbk );

    createToggleLabel ( sdf, gui, EDITSUBDIVIDERSYNC   , 230,  12,  20, 20,
                                                       subdivSyncCbk );

    createIntegerText ( sdf, gui, EDITSUBDIVIDERRENDER , 0, 8,
                                                         0,  24, 128, 32,
                                                       subdivRenderCbk );

    /*createToggleLabel ( sdf, gui, EDITMESHISOLINES     , 4,  96, 200, 20,
                                                       useIsoLinesCbk );*/
}


/******************************************************************************/
void updateSubdividerEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    /*** prevent a loop ***/
    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( strcmp ( child_name, EDITSUBDIVIDER ) == 0x00 ) {
            updateSubdivisionFrame ( child, gui );
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateSubdividerEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget *createSubdividerEdit ( GtkWidget *parent, G3DUI *gui,
                                                     char *name,
                                                     gint x,
                                                     gint y,
                                                     gint width,
                                                     gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *frm, *sdf, *fix;

    frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), gui );

    createSubdivisionFrame ( frm, gui,   0,   0, 286, 140 );

    gtk_widget_show ( frm );


    return frm;
}

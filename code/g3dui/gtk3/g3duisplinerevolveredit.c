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

static void updateSplineRevolverForm ( GtkWidget *, G3DUI * );

/******************************************************************************/
static void splineRevolverDivisCbk  ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent     ( widget );
    int level  = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( level >= 0x00 ) {
        common_g3duisplinerevolveredit_splineRevolverDivisCbk ( gui, level );
    }

    updateSplineRevolverForm ( parent, gui );
}

/******************************************************************************/
static void splineRevolverStepsCbk  ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int level  = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui    = ( G3DUI * ) user_data;

    if ( level >= 0x00 ) {
        common_g3duisplinerevolveredit_splineRevolverStepsCbk ( gui, level );
    }

    updateSplineRevolverForm ( parent, gui );
}

/******************************************************************************/
static void updateSplineRevolverForm ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DSPLINEREVOLVERTYPE ) ) {
        G3DSPLINEREVOLVER *srv = ( G3DSPLINEREVOLVER * ) obj;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( GTK_IS_SPIN_BUTTON(child) ) {
                GtkSpinButton *sbn = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITSPLINEREVOLVERSTEPS ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, srv->nbsteps );
                }

                if ( strcmp ( child_name, EDITSPLINEREVOLVERDIVIS ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, srv->nbdivis );
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
void updateSplineRevolverFrame ( GtkWidget *widget, G3DUI *gui ) {
    GtkWidget *frm = gtk_bin_get_child ( GTK_BIN(widget) );

    if ( frm ) updateSplineRevolverForm ( frm, gui );
}


/******************************************************************************/
static void createSplineRevolverFrame ( GtkWidget *frm, G3DUI *gui,
                                                     gint x,
                                                     gint y,
                                                     gint width,
                                                     gint height ) {
    GtkWidget *sdf;

    sdf = createFrame ( frm, gui, EDITSPLINEREVOLVER, x,   y, width, height );

    createIntegerText ( sdf, gui, EDITSPLINEREVOLVERSTEPS, 0,   0, 128, 32,
                                                       splineRevolverStepsCbk );

    createIntegerText ( sdf, gui, EDITSPLINEREVOLVERDIVIS, 0,  24, 128, 32,
                                                       splineRevolverDivisCbk );

    /*createToggleLabel ( sdf, gui, EDITMESHISOLINES     , 4,  96, 200, 20,
                                                       useIsoLinesCbk );*/
}


/******************************************************************************/
void updateSplineRevolverEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    /*** prevent a loop ***/
    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( strcmp ( child_name, EDITSPLINEREVOLVER ) == 0x00 ) {
            updateSplineRevolverFrame ( child, gui );
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateSplineRevolverEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget *createSplineRevolverEdit ( GtkWidget *parent, G3DUI *gui,
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

    createSplineRevolverFrame ( frm, gui,   0,   0, 286, 140 );

    gtk_widget_show ( frm );


    return frm;
}

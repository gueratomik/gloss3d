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

#define EDITSPLINEREVOLVER      "Spline Revolver"
#define EDITSPLINEREVOLVERSTEPS "Steps"
#define EDITSPLINEREVOLVERDIVIS "Divisions"

/******************************************************************************/
static GTK3G3DUISPLINEREVOLVEREDIT *gtk3_g3duisplinerevolveredit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUISPLINEREVOLVEREDIT *gtk3sed = calloc ( 0x01, sizeof ( GTK3G3DUISPLINEREVOLVEREDIT ) );

    if ( gtk3sed == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3sed->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3sed; 
}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUISPLINEREVOLVEREDIT *gtk3sed ) {
    gtk3sed->core.gui->lock = 0x01;

    if ( gtk3sed->core.editedSplineRevolver ) {
        G3DSPLINEREVOLVER *srv = gtk3sed->core.editedSplineRevolver;

        gtk_spin_button_set_value ( gtk3sed->stepsEntry    , srv->stepCount );
        gtk_spin_button_set_value ( gtk3sed->divisionsEntry, srv->divisionCount );
    }

    gtk3sed->core.gui->lock = 0x00;
}

/******************************************************************************/
static void splineRevolverStepsCbk ( GtkWidget *widget,
                                     gpointer   user_data ) {
    GTK3G3DUISPLINEREVOLVEREDIT *gtk3sed = ( GTK3G3DUISPLINEREVOLVEREDIT * ) user_data;
    G3DUI *gui = gtk3sed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int steps  = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    uint64_t ret;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3sed->core.gui->lock ) return;

    ret = g3duisplinerevolveredit_splineRevolverSteps ( &gtk3sed->core, steps );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
static void splineRevolverDivisCbk ( GtkWidget *widget,
                                     gpointer   user_data ) {
    GTK3G3DUISPLINEREVOLVEREDIT *gtk3sed = ( GTK3G3DUISPLINEREVOLVEREDIT * ) user_data;
    G3DUI *gui = gtk3sed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int divs  = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    uint64_t ret;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3sed->core.gui->lock ) return;

    ret = g3duisplinerevolveredit_splineRevolverDivis ( &gtk3sed->core, divs );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
void gtk3_g3duisplinerevolveredit_update ( GTK3G3DUISPLINEREVOLVEREDIT *gtk3sed ) {
    G3DUI *gui = gtk3sed->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3sed->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DSPLINEREVOLVERTYPE ) ) {
                gtk3sed->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3sed->core.editedSplineRevolver = ( G3DSPLINEREVOLVER * ) g3dscene_getLastSelected ( sce );

                if ( gtk3sed->core.editedSplineRevolver ) {
                    updateGeneralPanel  ( gtk3sed );
                }
            }
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3sed->notebook), FALSE );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void createGeneralPanel ( GTK3G3DUISPLINEREVOLVEREDIT *gtk3sed,
                                 gint               x,
                                 gint               y,
                                 gint               width,
                                 gint               height ) {
    GtkFixed *pan = ui_createTab ( gtk3sed->notebook,
                                   gtk3sed,
                                   EDITSPLINEREVOLVER,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3sed->stepsEntry       = ui_createIntegerText   ( pan, 
                                                         gtk3sed,
                                                         EDITSPLINEREVOLVERSTEPS,
                                                         CLASS_MAIN,
                                                         3, 256,
                                                         0,  0, 96, 96, 20,
                                                         splineRevolverStepsCbk );

    gtk3sed->divisionsEntry   = ui_createIntegerText   ( pan, 
                                                         gtk3sed,
                                                         EDITSPLINEREVOLVERDIVIS,
                                                         CLASS_MAIN,
                                                         3, 256,
                                                         0, 24, 96, 96, 20,
                                                         splineRevolverDivisCbk );

}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUISPLINEREVOLVEREDIT *gtk3sed = ( GTK3G3DUISPLINEREVOLVEREDIT * ) user_data;

    free ( gtk3sed );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUISPLINEREVOLVEREDIT *gtk3sed = ( GTK3G3DUISPLINEREVOLVEREDIT * ) user_data;

    gtk3_g3duisplinerevolveredit_update ( gtk3sed );
}

/******************************************************************************/
GTK3G3DUISPLINEREVOLVEREDIT *gtk3_g3duisplinerevolveredit_create ( GtkWidget *parent,
                                                                   GTK3G3DUI *gtk3gui,
                                                                   char      *name ) {
    GTK3G3DUISPLINEREVOLVEREDIT *gtk3sed = gtk3_g3duisplinerevolveredit_new ( gtk3gui );
    GtkWidget *notebook = ui_gtk_notebook_new ( CLASS_MAIN );

    gtk3sed->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3sed );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3sed );

    createGeneralPanel ( gtk3sed, 0, 0, 310, 150 );


    gtk_widget_show ( notebook );


    return gtk3sed;
}

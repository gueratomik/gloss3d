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

#define EDITBUCKETTOOLTOLERANCE "Tolerance"

/******************************************************************************/
static GTK3M3DUIBUCKETTOOLEDIT *gtk3_m3duibuckettooledit_new ( GTK3M3DUI *gtk3mui ) {
    uint32_t memsize = sizeof ( GTK3M3DUIBUCKETTOOLEDIT );
    GTK3M3DUIBUCKETTOOLEDIT *gtk3buckedit = calloc ( 0x01, memsize );

    if ( gtk3buckedit == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3buckedit->core.mui = ( M3DUI * ) gtk3mui;


    return gtk3buckedit; 
}

/******************************************************************************/
void gtk3_m3duibuckettooledit_update ( GTK3M3DUIBUCKETTOOLEDIT *gtk3buckedit ) {
    M3DUI *mui = gtk3buckedit->core.mui;
    G3DUI *gui = mui->gui;
    G3DUIMOUSETOOL *mou = g3dui_getMouseTool ( gui, BUCKETTOOL );

    gui->lock = 0x01;

    if ( mou ) {
        M3DMOUSETOOLBUCKET *mtb = ( M3DMOUSETOOLBUCKET * ) mou->tool;
        M3DBUCKET *bkt = mtb->ltool.obj;

        gtk_spin_button_set_value ( gtk3buckedit->toleranceEntry,
                                    bkt->tolerance );
    }

    gui->lock = 0x00;

}

/******************************************************************************/
static void setToleranceCbk  ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3M3DUIBUCKETTOOLEDIT *gtk3buckedit = ( GTK3M3DUIBUCKETTOOLEDIT * ) user_data;
    int tolerance = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    

    if ( gtk3buckedit->core.mui->gui->lock ) return;

    m3duibuckettooledit_setTolerance ( &gtk3buckedit->core, tolerance );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3M3DUIBUCKETTOOLEDIT *gtk3buckedit = ( GTK3M3DUIBUCKETTOOLEDIT * ) user_data;

    free ( gtk3buckedit );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3M3DUIBUCKETTOOLEDIT *gtk3buckedit = ( GTK3M3DUIBUCKETTOOLEDIT * ) user_data;

    gtk3_m3duibuckettooledit_update ( gtk3buckedit );
}

/******************************************************************************/
GTK3M3DUIBUCKETTOOLEDIT *gtk3_m3duibuckettooledit_create ( GtkWidget *parent, 
                                                           GTK3M3DUI *gtk3mui,
                                                           char      *name ) {
    GTK3M3DUIBUCKETTOOLEDIT *gtk3buckedit = gtk3_m3duibuckettooledit_new ( gtk3mui );

    gtk3buckedit->fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk3buckedit->toleranceEntry = ui_createIntegerText ( gtk3buckedit->fixed,
                                                          gtk3buckedit,
                                                          EDITBUCKETTOOLTOLERANCE, 
                                                          CLASS_MAIN,
                                                          0, 255,
                                                          0, 0, 96, 96, 20,
                                                          setToleranceCbk );

    g_signal_connect ( G_OBJECT (gtk3buckedit->fixed), "realize", G_CALLBACK (Realize), gtk3buckedit );
    g_signal_connect ( G_OBJECT (gtk3buckedit->fixed), "destroy", G_CALLBACK (Destroy), gtk3buckedit );


    gtk_widget_show ( gtk3buckedit->fixed );


    return gtk3buckedit;
}

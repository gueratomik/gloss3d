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

#define EDITCUBEGENERAL "Cube"
#define EDITCUBEXSLICES "X Slices"
#define EDITCUBEYSLICES "Y Slices"
#define EDITCUBEZSLICES "Z Slices"
#define EDITCUBERADIUS  "Radius"

/******************************************************************************/
static GTK3G3DUICUBEEDIT *gtk3_g3duicubeedit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUICUBEEDIT *gtk3ced = calloc ( 0x01, sizeof ( GTK3G3DUICUBEEDIT ) );

    if ( gtk3ced == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3ced->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3ced; 
}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUICUBEEDIT *gtk3ced ) {
    gtk3ced->core.gui->lock = 0x01;

    if ( gtk3ced->core.editedCube ) {
        G3DPRIMITIVE *pri = gtk3ced->core.editedCube;
        CUBEDATASTRUCT *cds = ( CUBEDATASTRUCT * ) pri->data;

        gtk_spin_button_set_value ( gtk3ced->xSlicesEntry, cds->nbx );
        gtk_spin_button_set_value ( gtk3ced->ySlicesEntry, cds->nby );
        gtk_spin_button_set_value ( gtk3ced->zSlicesEntry, cds->nbz );
        gtk_spin_button_set_value ( gtk3ced->radiusEntry , cds->radius );
    }

    gtk3ced->core.gui->lock = 0x00;
}

/******************************************************************************/
static void xsliceCbk ( GtkWidget *widget, 
                        gpointer   user_data ) {
    GTK3G3DUICUBEEDIT *gtk3ced = ( GTK3G3DUICUBEEDIT * ) user_data;
    G3DUI *gui = gtk3ced->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int slice = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ced->core.gui->lock ) return;

    if ( slice ) {
        g3duicubeedit_slice ( &gtk3ced->core, G3DUIXAXIS, slice );
    } else {
        gtk3_g3duicubeedit_update ( gtk3ced );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void ysliceCbk ( GtkWidget *widget,
                        gpointer   user_data ) {
    GTK3G3DUICUBEEDIT *gtk3ced = ( GTK3G3DUICUBEEDIT * ) user_data;
    G3DUI *gui = gtk3ced->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int slice = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ced->core.gui->lock ) return;

    if ( slice ) {
        g3duicubeedit_slice ( &gtk3ced->core, G3DUIYAXIS, slice );
    } else {
        gtk3_g3duicubeedit_update ( gtk3ced );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void zsliceCbk ( GtkWidget *widget,
                        gpointer   user_data ) {
    GTK3G3DUICUBEEDIT *gtk3ced = ( GTK3G3DUICUBEEDIT * ) user_data;
    G3DUI *gui = gtk3ced->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int slice = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ced->core.gui->lock ) return;

    if ( slice ) {
        g3duicubeedit_slice ( &gtk3ced->core, G3DUIZAXIS, slice );
    } else {
        gtk3_g3duicubeedit_update ( gtk3ced );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void radiusCbk ( GtkWidget *widget,
                        gpointer   user_data ) {
    GTK3G3DUICUBEEDIT *gtk3ced = ( GTK3G3DUICUBEEDIT * ) user_data;
    G3DUI *gui = gtk3ced->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ced->core.gui->lock ) return;

    g3duicubeedit_radius ( &gtk3ced->core, rad );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
void gtk3_g3duicubeedit_update ( GTK3G3DUICUBEEDIT *gtk3ced ) {
    G3DUI *gui = gtk3ced->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ced->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DCUBETYPE ) ) {
                gtk3ced->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3ced->core.editedCube = ( G3DPRIMITIVE * ) g3dscene_getLastSelected ( sce );

                if ( gtk3ced->core.editedCube ) {
                    updateGeneralPanel  ( gtk3ced );
                }
            }
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ced->notebook), FALSE );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void createGeneralPanel ( GTK3G3DUICUBEEDIT *gtk3ced,
                                 gint               x,
                                 gint               y,
                                 gint               width,
                                 gint               height ) {
    GtkFixed *pan = ui_createTab ( gtk3ced->notebook,
                                   gtk3ced,
                                   EDITCUBEGENERAL,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3ced->xSlicesEntry = ui_createIntegerText ( pan, 
                                                   gtk3ced,
                                                   EDITCUBEXSLICES,
                                                   CLASS_MAIN,
                                                   1, 100,
                                                   0, 0, 96, 96, 20,
                                                   xsliceCbk );

    gtk3ced->ySlicesEntry = ui_createIntegerText ( pan, 
                                                   gtk3ced,
                                                   EDITCUBEYSLICES,
                                                   CLASS_MAIN,
                                                   1, 100,
                                                   0, 24, 96, 96, 20,
                                                   ysliceCbk );

    gtk3ced->zSlicesEntry = ui_createIntegerText ( pan, 
                                                   gtk3ced,
                                                   EDITCUBEZSLICES,
                                                   CLASS_MAIN,
                                                   1, 100,
                                                   0, 48, 96, 96, 20,
                                                   zsliceCbk );

    gtk3ced->radiusEntry  = ui_createFloatText   ( pan, 
                                                   gtk3ced,
                                                   EDITCUBERADIUS,
                                                   CLASS_MAIN,
                                                   0.0f, FLT_MAX,
                                                   0, 72, 96, 96, 20,
                                                   radiusCbk );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUICUBEEDIT *gtk3ced = ( GTK3G3DUICUBEEDIT * ) user_data;

    free ( gtk3ced );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUICUBEEDIT *gtk3ced = ( GTK3G3DUICUBEEDIT * ) user_data;

    gtk3_g3duicubeedit_update ( gtk3ced );
}

/******************************************************************************/
GTK3G3DUICUBEEDIT *gtk3_g3duicubeedit_create ( GtkWidget *parent,
                                               GTK3G3DUI *gtk3gui,
                                               char      *name ) {
    GTK3G3DUICUBEEDIT *gtk3ced = gtk3_g3duicubeedit_new ( gtk3gui );
    GtkWidget *notebook = gtk_notebook_new ( );

    gtk3ced->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3ced );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3ced );

    createGeneralPanel ( gtk3ced, 0, 0, 310, 150 );


    gtk_widget_show ( notebook );


    return gtk3ced;
}

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

#define EDITCUTMESHTOOLRESTRICT    "Restrict to selection"

/******************************************************************************/
static GTK3G3DUICUTMESHTOOLEDIT *gtk3_g3duicutmeshtooledit_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memsize = sizeof ( GTK3G3DUICUTMESHTOOLEDIT );
    GTK3G3DUICUTMESHTOOLEDIT *gtk3cmedit = calloc ( 0x01, memsize );

    if ( gtk3cmedit == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3cmedit->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3cmedit; 
}

/******************************************************************************/
static void restrictCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUICUTMESHTOOLEDIT *gtk3cmedit = ( GTK3G3DUICUTMESHTOOLEDIT * ) user_data;
    int restricted = (int) gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget));

    /*** prevents a loop ***/
    if ( gtk3cmedit->core.gui->lock ) return;

    g3duicutmeshtooledit_restrictCbk ( &gtk3cmedit->core, restricted );
}

/******************************************************************************/
void gtk3_g3duicutmeshtooledit_update ( GTK3G3DUICUTMESHTOOLEDIT *gtk3cmedit ) {
    G3DUI *gui = gtk3cmedit->core.gui;
    G3DUIMOUSETOOL *mtl = gui->curmou;

    /*** prevent a loop ***/
    gtk3cmedit->core.gui->lock = 0x01;

    if ( mtl ) {
        G3DMOUSETOOLCUTMESH *cm = ( G3DMOUSETOOLCUTMESH * ) mtl->tool;

        if ( cm->restrict_to_selection ) {
            gtk_toggle_button_set_active ( gtk3cmedit->restrictToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3cmedit->restrictToggle, FALSE );
        }
    }

    gtk3cmedit->core.gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUICUTMESHTOOLEDIT *gtk3cmedit = ( GTK3G3DUICUTMESHTOOLEDIT * ) user_data;

    free ( gtk3cmedit );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUICUTMESHTOOLEDIT *gtk3cmedit = ( GTK3G3DUICUTMESHTOOLEDIT * ) user_data;

    gtk3_g3duicutmeshtooledit_update ( gtk3cmedit );
}

/******************************************************************************/
GTK3G3DUICUTMESHTOOLEDIT *gtk3_g3duicutmeshtooledit_create ( GtkWidget *parent, 
                                                             GTK3G3DUI *gtk3gui,
                                                             char      *name ) {
    GTK3G3DUICUTMESHTOOLEDIT *gtk3cmedit = gtk3_g3duicutmeshtooledit_new ( gtk3gui );
    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk3cmedit->fixed = fixed;

    gtk_widget_set_size_request ( gtk3cmedit->fixed, 310, 150 );

    g_signal_connect ( G_OBJECT (fixed), "realize", G_CALLBACK (Realize), gtk3cmedit );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3cmedit );

    gtk3cmedit->restrictToggle = ui_createToggleLabel ( fixed, 
                                                        gtk3cmedit,
                                                        EDITCUTMESHTOOLRESTRICT,
                                                        CLASS_MAIN,
                                                        0, 0, 144, 20, 20,
                                                        restrictCbk );


    gtk_widget_show ( fixed );


    return gtk3cmedit;
}

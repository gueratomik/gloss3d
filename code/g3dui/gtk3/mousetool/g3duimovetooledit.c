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

#define EDITMOVETOOLVISIBLE        "Pick only visible"

/******************************************************************************/
static GTK3G3DUIMOVETOOLEDIT *gtk3_g3duimovetooledit_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memsize = sizeof ( GTK3G3DUIMOVETOOLEDIT );
    GTK3G3DUIMOVETOOLEDIT *gtk3moveedit = calloc ( 0x01, memsize );

    if ( gtk3moveedit == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3moveedit->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3moveedit; 
}

/******************************************************************************/
static void onlyVisibleCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMOVETOOLEDIT *gtk3moveedit = ( GTK3G3DUIMOVETOOLEDIT * ) user_data;
    int visible_only = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    /*** prevents a loop ***/
    if ( gtk3moveedit->core.gui->lock ) return;

    g3duimovetooledit_onlyVisibleCbk ( &gtk3moveedit->core, visible_only );
}

/******************************************************************************/
void gtk3_g3duimovetooledit_update ( GTK3G3DUIMOVETOOLEDIT *gtk3moveedit ) {
    G3DUI *gui = gtk3moveedit->core.gui;
    G3DUIMOUSETOOL *mtl = gui->curmou;

    /*** prevent a loop ***/
    gtk3moveedit->core.gui->lock = 0x01;

    if ( mtl ) {
        G3DMOUSETOOLMOVE *pt = ( G3DMOUSETOOLMOVE * ) mtl->tool;

        if ( pt->only_visible ) {
            gtk_toggle_button_set_active ( gtk3moveedit->visibleToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3moveedit->visibleToggle, FALSE );
        }
    }

    gtk3moveedit->core.gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMOVETOOLEDIT *gtk3moveedit = ( GTK3G3DUIMOVETOOLEDIT * ) user_data;

    free ( gtk3moveedit );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMOVETOOLEDIT *gtk3moveedit = ( GTK3G3DUIMOVETOOLEDIT * ) user_data;

    gtk3_g3duimovetooledit_update ( gtk3moveedit );
}

/******************************************************************************/
GTK3G3DUIMOVETOOLEDIT *gtk3_g3duimovetooledit_create ( GtkWidget *parent, 
                                                       GTK3G3DUI *gtk3gui,
                                                       char      *name ) {
    GTK3G3DUIMOVETOOLEDIT *gtk3moveedit = gtk3_g3duimovetooledit_new ( gtk3gui );

    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN );
    GtkWidget *weightFixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk3moveedit->fixed = fixed;

    gtk_widget_set_size_request ( gtk3moveedit->fixed, 310, 150 );

    g_signal_connect ( G_OBJECT (fixed), "realize", G_CALLBACK (Realize), gtk3moveedit );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3moveedit );

    gtk3moveedit->visibleToggle       = ui_createToggleLabel ( fixed, 
                                                               gtk3moveedit,
                                                               EDITMOVETOOLVISIBLE,
                                                               CLASS_MAIN,
                                                               0, 0, 120, 20, 20,
                                                               onlyVisibleCbk );

    gtk_widget_show ( fixed );


    return gtk3moveedit;
}

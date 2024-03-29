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

#define EDITWIREFRAMEGENERAL   "Wireframe"
#define EDITWIREFRAMETHICKNESS "Thickness"

/******************************************************************************/
static GTK3G3DUIWIREFRAMEEDIT *gtk3_g3duiwireframeedit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUIWIREFRAMEEDIT *gtk3wed = calloc ( 0x01, sizeof ( GTK3G3DUIWIREFRAMEEDIT ) );

    if ( gtk3wed == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3wed->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3wed; 
}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUIWIREFRAMEEDIT *gtk3wed ) {
    gtk3wed->core.gui->lock = 0x01;

    if ( gtk3wed->core.editedWireframe ) {
        G3DWIREFRAME *wir = gtk3wed->core.editedWireframe;

        gtk_spin_button_set_value ( gtk3wed->thicknessEntry, wir->thickness );
    }

    gtk3wed->core.gui->lock = 0x00;
}

/******************************************************************************/
static void thicknessCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIWIREFRAMEEDIT *gtk3wed = ( GTK3G3DUIWIREFRAMEEDIT * ) user_data;
    G3DUI *gui = gtk3wed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    double thickness = gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    g3duiwireframeedit_thickness ( &gtk3wed->core, thickness );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
void gtk3_g3duiwireframeedit_update ( GTK3G3DUIWIREFRAMEEDIT *gtk3wed ) {
    G3DUI *gui = gtk3wed->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3wed->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DWIREFRAMETYPE ) ) {
                gtk3wed->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3wed->core.editedWireframe = ( G3DPRIMITIVE * ) g3dscene_getLastSelected ( sce );

                if ( gtk3wed->core.editedWireframe ) {
                    updateGeneralPanel  ( gtk3wed );
                }
            }
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3wed->notebook), FALSE );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void createGeneralPanel ( GTK3G3DUIWIREFRAMEEDIT *gtk3wed,
                                 gint                    x,
                                 gint                    y,
                                 gint                    width,
                                 gint                    height ) {
    GtkFixed *pan = ui_createTab ( gtk3wed->notebook,
                                   gtk3wed,
                                   EDITWIREFRAMEGENERAL,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3wed->thicknessEntry  = ui_createFloatText   ( pan, 
                                                      gtk3wed,
                                                      EDITWIREFRAMETHICKNESS,
                                                      CLASS_MAIN,
                                                      0.0f, FLT_MAX,
                                                      0, 0, 96, 96, 20,
                                                      thicknessCbk );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIWIREFRAMEEDIT *gtk3wed = ( GTK3G3DUIWIREFRAMEEDIT * ) user_data;

    free ( gtk3wed );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIWIREFRAMEEDIT *gtk3wed = ( GTK3G3DUIWIREFRAMEEDIT * ) user_data;

    gtk3_g3duiwireframeedit_update ( gtk3wed );
}

/******************************************************************************/
GTK3G3DUIWIREFRAMEEDIT *gtk3_g3duiwireframeedit_create ( GtkWidget *parent,
                                                         GTK3G3DUI *gtk3gui,
                                                         char      *name ) {
    GTK3G3DUIWIREFRAMEEDIT *gtk3wed = gtk3_g3duiwireframeedit_new ( gtk3gui );
    GtkWidget *notebook = ui_gtk_notebook_new ( CLASS_MAIN );

    gtk3wed->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3wed );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3wed );

    createGeneralPanel ( gtk3wed, 0, 0, 310, 150 );


    gtk_widget_show ( notebook );


    return gtk3wed;
}

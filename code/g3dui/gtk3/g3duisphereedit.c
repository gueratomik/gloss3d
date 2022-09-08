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

#define EDITSPHEREGENERAL "Sphere"
#define EDITSPHERECAPS    "Caps"
#define EDITSPHERESLICES  "Slices"
#define EDITSPHERERADIUS  "Radius"
#define EDITSPHEREPERFECT "Render Perfect"

/******************************************************************************/
static GTK3G3DUISPHEREEDIT *gtk3_g3duisphereedit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUISPHEREEDIT *gtk3sed = calloc ( 0x01, sizeof ( GTK3G3DUISPHEREEDIT ) );

    if ( gtk3sed == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3sed->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3sed; 
}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUISPHEREEDIT *gtk3sed ) {
    gtk3sed->core.gui->lock = 0x01;

    if ( gtk3sed->core.editedSphere ) {
        G3DPRIMITIVE *pri = gtk3sed->core.editedSphere;
        SPHEREDATASTRUCT *sds = ( SPHEREDATASTRUCT * ) pri->data;

        gtk_spin_button_set_value ( gtk3sed->capsEntry  , sds->cap    );
        gtk_spin_button_set_value ( gtk3sed->slicesEntry, sds->slice  );
        gtk_spin_button_set_value ( gtk3sed->radiusEntry, sds->radius );

        if ( pri->mes.obj.flags & SPHEREISPERFECT ) {
            gtk_toggle_button_set_active ( gtk3sed->perfectToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3sed->perfectToggle, FALSE );
        }
    }

    gtk3sed->core.gui->lock = 0x00;
}

/******************************************************************************/
static void togglePerfectCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUISPHEREEDIT *gtk3sed = ( GTK3G3DUISPHEREEDIT * ) user_data;
    G3DUI *gui = gtk3sed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3sed->core.gui->lock ) return;

    g3duisphereedit_togglePerfect ( &gtk3sed->core );
}

/******************************************************************************/
static void radiusCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUISPHEREEDIT *gtk3sed = ( GTK3G3DUISPHEREEDIT * ) user_data;
    G3DUI *gui = gtk3sed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3sed->core.gui->lock ) return;

    g3duisphereedit_radius ( &gtk3sed->core, rad );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void capCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUISPHEREEDIT *gtk3sed = ( GTK3G3DUISPHEREEDIT * ) user_data;
    G3DUI *gui = gtk3sed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int cap = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3sed->core.gui->lock ) return;

    if ( cap >= 0x03 ) {
        g3duisphereedit_cap ( &gtk3sed->core, cap );
    } else {
        gtk3_g3duisphereedit_update ( gtk3sed );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void sliceCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUISPHEREEDIT *gtk3sed = ( GTK3G3DUISPHEREEDIT * ) user_data;
    G3DUI *gui = gtk3sed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int slice = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3sed->core.gui->lock ) return;

    if ( slice >= 0x03 ) {
        g3duisphereedit_slice ( &gtk3sed->core, slice );
    } else {
        gtk3_g3duisphereedit_update ( gtk3sed );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
void gtk3_g3duisphereedit_update ( GTK3G3DUISPHEREEDIT *gtk3sed ) {
    G3DUI *gui = gtk3sed->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3sed->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DSPHERETYPE ) ) {
                gtk3sed->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3sed->core.editedSphere = ( G3DPRIMITIVE * ) g3dscene_getLastSelected ( sce );

                if ( gtk3sed->core.editedSphere ) {
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
static void createGeneralPanel ( GTK3G3DUISPHEREEDIT *gtk3sed,
                                 gint               x,
                                 gint               y,
                                 gint               width,
                                 gint               height ) {
    GtkFixed *pan = ui_createTab ( gtk3sed->notebook,
                                   gtk3sed,
                                   EDITSPHEREGENERAL,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3sed->capsEntry       = ui_createIntegerText ( pan, 
                                                      gtk3sed,
                                                      EDITSPHERECAPS,
                                                      CLASS_MAIN,
                                                      3, 256,
                                                      0, 0, 96, 96, 20,
                                                      capCbk );

    gtk3sed->slicesEntry     = ui_createIntegerText ( pan, 
                                                      gtk3sed,
                                                      EDITSPHERESLICES,
                                                      CLASS_MAIN,
                                                      3, 256,
                                                      0, 24, 96, 96, 20,
                                                      sliceCbk );

    gtk3sed->radiusEntry     = ui_createFloatText   ( pan, 
                                                      gtk3sed,
                                                      EDITSPHERERADIUS,
                                                      CLASS_MAIN,
                                                      0.0f, FLT_MAX,
                                                      0, 48, 96, 96, 20,
                                                      radiusCbk );

    gtk3sed->perfectToggle   = ui_createToggleLabel ( pan, 
                                                      gtk3sed,
                                                      EDITSPHEREPERFECT,
                                                      CLASS_MAIN,
                                                      0, 72, 96, 96, 20,
                                                      togglePerfectCbk );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUISPHEREEDIT *gtk3sed = ( GTK3G3DUISPHEREEDIT * ) user_data;

    free ( gtk3sed );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUISPHEREEDIT *gtk3sed = ( GTK3G3DUISPHEREEDIT * ) user_data;

    gtk3_g3duisphereedit_update ( gtk3sed );
}

/******************************************************************************/
GTK3G3DUISPHEREEDIT *gtk3_g3duisphereedit_create ( GtkWidget *parent,
                                                 GTK3G3DUI *gtk3gui,
                                                 char      *name ) {
    GTK3G3DUISPHEREEDIT *gtk3sed = gtk3_g3duisphereedit_new ( gtk3gui );
    GtkWidget *notebook = gtk_notebook_new ( );

    gtk3sed->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3sed );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3sed );

    createGeneralPanel ( gtk3sed, 0, 0, 310, 150 );


    gtk_widget_show ( notebook );


    return gtk3sed;
}

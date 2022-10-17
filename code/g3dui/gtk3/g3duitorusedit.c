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

#define EDITTORUSGENERAL     "Torus"
#define EDITTORUSSLICES      "Slices"
#define EDITTORUSCAPS        "Caps"
#define EDITTORUSEXTRAD      "Outer Radius"
#define EDITTORUSINTRAD      "Inner Radius"
#define EDITTORUSORIENTATION "Orientation"

/******************************************************************************/
static GTK3G3DUITORUSEDIT *gtk3_g3duitorusedit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUITORUSEDIT *gtk3ted = calloc ( 0x01, sizeof ( GTK3G3DUITORUSEDIT ) );

    if ( gtk3ted == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3ted->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3ted; 
}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUITORUSEDIT *gtk3ted ) {
    gtk3ted->core.gui->lock = 0x01;

    if ( gtk3ted->core.editedTorus ) {
        G3DPRIMITIVE *pri = gtk3ted->core.editedTorus;
        TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;

        gtk_spin_button_set_value ( gtk3ted->capsEntry   , tds->cap         );
        gtk_spin_button_set_value ( gtk3ted->slicesEntry , tds->slice       );
        gtk_spin_button_set_value ( gtk3ted->extradEntry , tds->extrad      );
        gtk_spin_button_set_value ( gtk3ted->intradEntry , tds->intrad      );
        gtk_combo_box_set_active  ( GTK_COMBO_BOX(gtk3ted->axisSelector), tds->orientation );
    }

    gtk3ted->core.gui->lock = 0x00;
}

/******************************************************************************/
static void intRadiusCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITORUSEDIT *gtk3ted = ( GTK3G3DUITORUSEDIT * ) user_data;
    G3DUI *gui = gtk3ted->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ted->core.gui->lock ) return;

    g3duitorusedit_intRadius ( &gtk3ted->core, rad );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void extRadiusCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITORUSEDIT *gtk3ted = ( GTK3G3DUITORUSEDIT * ) user_data;
    G3DUI *gui = gtk3ted->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ted->core.gui->lock ) return;

    g3duitorusedit_extRadius ( &gtk3ted->core, rad );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void capCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITORUSEDIT *gtk3ted = ( GTK3G3DUITORUSEDIT * ) user_data;
    G3DUI *gui = gtk3ted->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int cap = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ted->core.gui->lock ) return;

    if ( cap >= 0x03 ) {
        g3duitorusedit_cap ( &gtk3ted->core, cap );
    } else {
        gtk3_g3duitorusedit_update ( gtk3ted );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void sliceCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITORUSEDIT *gtk3ted = ( GTK3G3DUITORUSEDIT * ) user_data;
    G3DUI *gui = gtk3ted->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int slice = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ted->core.gui->lock ) return;

    if ( slice >= 0x03 ) {
        g3duitorusedit_slice ( &gtk3ted->core, slice );
    } else {
        gtk3_g3duitorusedit_update ( gtk3ted );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void orientationCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITORUSEDIT *gtk3ted = ( GTK3G3DUITORUSEDIT * ) user_data;
    G3DUI *gui = gtk3ted->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    GtkComboBoxText *cmb = GTK_COMBO_BOX_TEXT(widget);
    const char *str = gtk_combo_box_text_get_active_text ( cmb );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ted->core.gui->lock ) return;

    g3duitorusedit_orientation ( &gtk3ted->core, str );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
void gtk3_g3duitorusedit_update ( GTK3G3DUITORUSEDIT *gtk3ted ) {
    G3DUI *gui = gtk3ted->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ted->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DTORUSTYPE ) ) {
                gtk3ted->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3ted->core.editedTorus = ( G3DPRIMITIVE * ) g3dscene_getLastSelected ( sce );

                if ( gtk3ted->core.editedTorus ) {
                    updateGeneralPanel  ( gtk3ted );
                }
            }
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ted->notebook), FALSE );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void createGeneralPanel ( GTK3G3DUITORUSEDIT *gtk3ted,
                                 gint               x,
                                 gint               y,
                                 gint               width,
                                 gint               height ) {
    GtkFixed *pan = ui_createTab ( gtk3ted->notebook,
                                   gtk3ted,
                                   EDITTORUSGENERAL,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3ted->capsEntry       = ui_createIntegerText ( pan, 
                                                      gtk3ted,
                                                      EDITTORUSCAPS,
                                                      CLASS_MAIN,
                                                      3, 256,
                                                      0, 0, 96, 96, 20,
                                                      capCbk );

    gtk3ted->slicesEntry     = ui_createIntegerText ( pan, 
                                                      gtk3ted,
                                                      EDITTORUSSLICES,
                                                      CLASS_MAIN,
                                                      3, 256,
                                                      0, 24, 96, 96, 20,
                                                      sliceCbk );

    gtk3ted->extradEntry     = ui_createFloatText   ( pan, 
                                                      gtk3ted,
                                                      EDITTORUSEXTRAD,
                                                      CLASS_MAIN,
                                                      0.0f, FLT_MAX,
                                                      0, 48, 96, 96, 20,
                                                      extRadiusCbk );

    gtk3ted->intradEntry     = ui_createFloatText   ( pan, 
                                                      gtk3ted,
                                                      EDITTORUSINTRAD,
                                                      CLASS_MAIN,
                                                      0.0f, FLT_MAX,
                                                      0, 72, 96, 96, 20,
                                                      intRadiusCbk );

    gtk3ted->axisSelector   = ui_createPlaneSelector ( pan, 
                                                       gtk3ted, 
                                                       EDITTORUSORIENTATION,  
                                                       CLASS_MAIN,
                                                       0, 96, 96, 96, 20,
                                                       orientationCbk );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUITORUSEDIT *gtk3ted = ( GTK3G3DUITORUSEDIT * ) user_data;

    free ( gtk3ted );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUITORUSEDIT *gtk3ted = ( GTK3G3DUITORUSEDIT * ) user_data;

    gtk3_g3duitorusedit_update ( gtk3ted );
}

/******************************************************************************/
GTK3G3DUITORUSEDIT *gtk3_g3duitorusedit_create ( GtkWidget *parent,
                                                 GTK3G3DUI *gtk3gui,
                                                 char      *name ) {
    GTK3G3DUITORUSEDIT *gtk3ted = gtk3_g3duitorusedit_new ( gtk3gui );
    GtkWidget *notebook = ui_gtk_notebook_new ( CLASS_MAIN );

    gtk3ted->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3ted );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3ted );

    createGeneralPanel ( gtk3ted, 0, 0, 310, 150 );


    gtk_widget_show ( notebook );


    return gtk3ted;
}

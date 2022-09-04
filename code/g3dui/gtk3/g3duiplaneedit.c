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

#define EDITPLANEGENERAL     "Plane"
#define EDITPLANEDIVU        "U Divisions"
#define EDITPLANEDIVV        "V Divisions"
#define EDITPLANERADU        "U Radius"
#define EDITPLANERADV        "V Radius"
#define EDITPLANEORIENTATION "Orientation"

/******************************************************************************/
static GTK3G3DUIPLANEEDIT *gtk3_g3duiplaneedit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUIPLANEEDIT *gtk3ped = calloc ( 0x01, sizeof ( GTK3G3DUIPLANEEDIT ) );

    if ( gtk3ped == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3ped->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3ped; 
}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUIPLANEEDIT *gtk3ped ) {
    gtk3ped->core.gui->lock = 0x01;

    if ( gtk3ped->core.editedPlane ) {
        G3DPRIMITIVE *pri = gtk3ped->core.editedPlane;
        PLANEDATASTRUCT *pds = ( PLANEDATASTRUCT * ) pri->data;

        gtk_spin_button_set_value ( gtk3ped->divUEntry   , pds->nbu         );
        gtk_spin_button_set_value ( gtk3ped->divVEntry   , pds->nbv         );
        gtk_spin_button_set_value ( gtk3ped->radiusUEntry, pds->radu        );
        gtk_spin_button_set_value ( gtk3ped->radiusVEntry, pds->radv        );
        gtk_combo_box_set_active  ( GTK_COMBO_BOX(gtk3ped->axisSelector), pds->orientation );
    }

    gtk3ped->core.gui->lock = 0x00;
}

/******************************************************************************/
static void divvCbk ( GtkWidget *widget,
                        gpointer   user_data ) {
    GTK3G3DUIPLANEEDIT *gtk3ped = ( GTK3G3DUIPLANEEDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int div = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiplaneedit_divCbk ( &gtk3ped->core, G3DUIVAXIS, div );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void divuCbk ( GtkWidget *widget,
                        gpointer   user_data ) {
    GTK3G3DUIPLANEEDIT *gtk3ped = ( GTK3G3DUIPLANEEDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int div = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiplaneedit_divCbk ( &gtk3ped->core, G3DUIUAXIS, div );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void radvCbk ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIPLANEEDIT *gtk3ped = ( GTK3G3DUIPLANEEDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiplaneedit_radiusCbk ( &gtk3ped->core, G3DUIUAXIS, rad );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void raduCbk ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIPLANEEDIT *gtk3ped = ( GTK3G3DUIPLANEEDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiplaneedit_radiusCbk ( &gtk3ped->core, G3DUIVAXIS, rad );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void orientationCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPLANEEDIT *gtk3ped = ( GTK3G3DUIPLANEEDIT * ) user_data;
    G3DUI *gui = gtk3ped->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    GtkComboBoxText *cmb = GTK_COMBO_BOX_TEXT(widget);
    const char *str = gtk_combo_box_text_get_active_text ( cmb );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3duiplaneedit_orientationCbk ( &gtk3ped->core, str );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
void gtk3_g3duiplaneedit_update ( GTK3G3DUIPLANEEDIT *gtk3ped ) {
    G3DUI *gui = gtk3ped->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ped->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DPLANETYPE ) ) {
                gtk3ped->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3ped->core.editedPlane = ( G3DPRIMITIVE * ) g3dscene_getLastSelected ( sce );

                if ( gtk3ped->core.editedPlane ) {
                    updateGeneralPanel  ( gtk3ped );
                }
            }
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ped->notebook), FALSE );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void createGeneralPanel ( GTK3G3DUIPLANEEDIT *gtk3ped,
                                 gint               x,
                                 gint               y,
                                 gint               width,
                                 gint               height ) {
    GtkFixed *pan = ui_createTab ( gtk3ped->notebook,
                                   gtk3ped,
                                   EDITPLANEGENERAL,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3ped->divUEntry = ui_createIntegerText ( pan, 
                                                   gtk3ped,
                                                   EDITPLANEDIVU,
                                                   CLASS_MAIN,
                                                   1, 100,
                                                   0, 0, 96, 96, 20,
                                                   divuCbk );

    gtk3ped->divVEntry = ui_createIntegerText ( pan, 
                                                   gtk3ped,
                                                   EDITPLANEDIVV,
                                                   CLASS_MAIN,
                                                   1, 100,
                                                   0, 24, 96, 96, 20,
                                                   divvCbk );

    gtk3ped->radiusUEntry = ui_createFloatText ( pan, 
                                                   gtk3ped,
                                                   EDITPLANERADU,
                                                   CLASS_MAIN,
                                                   0.0f, FLT_MAX,
                                                   0, 48, 96, 96, 20,
                                                   raduCbk );

    gtk3ped->radiusVEntry  = ui_createFloatText   ( pan, 
                                                   gtk3ped,
                                                   EDITPLANERADV,
                                                   CLASS_MAIN,
                                                   0.0f, FLT_MAX,
                                                   0, 72, 96, 96, 20,
                                                   radvCbk );

    gtk3ped->axisSelector  = ui_createPlaneSelector ( pan, 
                                                      gtk3ped, 
                                                      EDITPLANEORIENTATION,  
                                                      CLASS_MAIN,
                                                      0, 96, 96, 96, 20,
                                                      orientationCbk );

}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIPLANEEDIT *gtk3ped = ( GTK3G3DUIPLANEEDIT * ) user_data;

    free ( gtk3ped );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIPLANEEDIT *gtk3ped = ( GTK3G3DUIPLANEEDIT * ) user_data;

    gtk3_g3duiplaneedit_update ( gtk3ped );
}

/******************************************************************************/
GTK3G3DUIPLANEEDIT *gtk3_g3duiplaneedit_create ( GtkWidget *parent,
                                               GTK3G3DUI *gtk3gui,
                                               char      *name ) {
    GTK3G3DUIPLANEEDIT *gtk3ped = gtk3_g3duiplaneedit_new ( gtk3gui );
    GtkWidget *notebook = gtk_notebook_new ( );

    gtk3ped->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3ped );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3ped );

    createGeneralPanel ( gtk3ped, 0, 0, 310, 150 );


    gtk_widget_show ( notebook );


    return gtk3ped;
}

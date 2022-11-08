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

#define EDITSCULPTTOOLVISIBLE  "Only visible"
#define EDITSCULPTTOOLRADIUS   "Radius"
#define EDITSCULPTTOOLPRESSURE "Pressure"
#define EDITSCULPTTOOLSQUARED  "Squared pattern"
#define EDITSCULPTTOOLCIRCULAR "Circular pattern"

/******************************************************************************/
static GTK3G3DUISCULPTTOOLEDIT *gtk3_g3duisculpttooledit_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memsize = sizeof ( GTK3G3DUISCULPTTOOLEDIT );
    GTK3G3DUISCULPTTOOLEDIT *gtk3sculptedit = calloc ( 0x01, memsize );

    if ( gtk3sculptedit == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3sculptedit->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3sculptedit; 
}

/******************************************************************************/
static void setRadiusCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUISCULPTTOOLEDIT *gtk3sculptedit = ( GTK3G3DUISCULPTTOOLEDIT * ) user_data;
    int radius = ( int ) gtk_range_get_value ( GTK_RANGE(widget) );

    /*** prevents a loop ***/
    if ( gtk3sculptedit->core.gui->lock ) return;

    g3duisculpttooledit_setRadiusCbk ( &gtk3sculptedit->core, radius );
}

/******************************************************************************/
static void setPressureCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUISCULPTTOOLEDIT *gtk3sculptedit = ( GTK3G3DUISCULPTTOOLEDIT * ) user_data;
    float pressure = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );

    /*** prevents a loop ***/
    if ( gtk3sculptedit->core.gui->lock ) return;

    g3duisculpttooledit_setPressureCbk ( &gtk3sculptedit->core, pressure / 100.0f );
}

/******************************************************************************/
static void onlyVisibleCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUISCULPTTOOLEDIT *gtk3sculptedit = ( GTK3G3DUISCULPTTOOLEDIT * ) user_data;
    int visible_only = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    /*** prevents a loop ***/
    if ( gtk3sculptedit->core.gui->lock ) return;

    g3duisculpttooledit_onlyVisibleCbk ( &gtk3sculptedit->core, visible_only );
}

/******************************************************************************/
static void setCircularCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3G3DUISCULPTTOOLEDIT *gtk3sculptedit = ( GTK3G3DUISCULPTTOOLEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3sculptedit->core.gui;

    /*** prevents a loop ***/
    if ( gtk3sculptedit->core.gui->lock ) return;

    g3duisculpttooledit_setCircularCbk ( &gtk3sculptedit->core );
    
    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMOUSETOOL );
}

/******************************************************************************/
static void unsetCircularCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUISCULPTTOOLEDIT *gtk3sculptedit = ( GTK3G3DUISCULPTTOOLEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3sculptedit->core.gui;
    
    /*** prevents a loop ***/
    if ( gtk3sculptedit->core.gui->lock ) return;

    g3duisculpttooledit_unsetCircularCbk ( &gtk3sculptedit->core );
    
    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMOUSETOOL );
}


/******************************************************************************/
void gtk3_g3duisculpttooledit_update ( GTK3G3DUISCULPTTOOLEDIT *gtk3sculptedit ) {
    G3DUI *gui = gtk3sculptedit->core.gui;
    G3DUIMOUSETOOL *mtl = gui->curmou;

    /*** prevent a loop ***/
    gtk3sculptedit->core.gui->lock = 0x01;

    gtk_widget_set_sensitive ( gtk3sculptedit->visibleToggle, FALSE );
    gtk_widget_set_sensitive ( gtk3sculptedit->radiusScale  , FALSE );
    gtk_widget_set_sensitive ( gtk3sculptedit->pressureScale, FALSE );

    if ( mtl ) {
        G3DMOUSETOOLSCULPT *sc = ( G3DMOUSETOOLSCULPT * ) mtl->tool;

        gtk_widget_set_sensitive ( gtk3sculptedit->visibleToggle, TRUE );
        gtk_widget_set_sensitive ( gtk3sculptedit->radiusScale  , TRUE );
        gtk_widget_set_sensitive ( gtk3sculptedit->pressureScale, TRUE );

        if ( sc->only_visible ) {
            gtk_toggle_button_set_active ( gtk3sculptedit->visibleToggle , TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3sculptedit->visibleToggle , FALSE );
        }

        if ( sc->circular == 0x01 ) {
            gtk_toggle_button_set_active ( gtk3sculptedit->circularToggle, TRUE  );
            gtk_toggle_button_set_active ( gtk3sculptedit->squaredToggle , FALSE );
        } else {
            gtk_toggle_button_set_active ( gtk3sculptedit->circularToggle, FALSE );
            gtk_toggle_button_set_active ( gtk3sculptedit->squaredToggle , TRUE  );
        }

        gtk_range_set_value ( gtk3sculptedit->radiusScale  , sc->radius );
        gtk_range_set_value ( gtk3sculptedit->pressureScale, sc->pressure * 100.0f );

        switch ( sc->type ) {
            case SCULPTINFLATE :

            break;

            case SCULPTCREASE :

            break;

            case SCULPTFLATTEN :

            break;

            case SCULPTSMOOTH :

            break;

            case SCULPTUNSCULPT :

            break;

            default :

            break;
        }
    }

    gtk3sculptedit->core.gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUISCULPTTOOLEDIT *gtk3sculptedit = ( GTK3G3DUISCULPTTOOLEDIT * ) user_data;

    free ( gtk3sculptedit );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUISCULPTTOOLEDIT *gtk3sculptedit = ( GTK3G3DUISCULPTTOOLEDIT * ) user_data;

    gtk3_g3duisculpttooledit_update ( gtk3sculptedit );
}

/******************************************************************************/
GTK3G3DUISCULPTTOOLEDIT *gtk3_g3duisculpttooledit_create ( GtkWidget *parent, 
                                                           GTK3G3DUI *gtk3gui,
                                                           char      *name ) {
    GTK3G3DUISCULPTTOOLEDIT *gtk3sculptedit = gtk3_g3duisculpttooledit_new ( gtk3gui );

    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk3sculptedit->fixed = fixed;

    gtk_widget_set_size_request ( gtk3sculptedit->fixed, 310, 150 );

    g_signal_connect ( G_OBJECT (fixed), "realize", G_CALLBACK (Realize), gtk3sculptedit );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3sculptedit );

    gtk3sculptedit->visibleToggle  = ui_createToggleLabel    ( fixed,
                                                               gtk3sculptedit,
                                                               EDITSCULPTTOOLVISIBLE,
                                                               CLASS_MAIN,
                                                               0,   0, 124, 20, 20,
                                                               onlyVisibleCbk  );

          /*** Use image as texture ***/
    gtk3sculptedit->squaredToggle  = ui_createToggleLabel    ( fixed,
                                                               gtk3sculptedit,
                                                               EDITSCULPTTOOLSQUARED,
                                                               CLASS_MAIN,
                                                               0, 24, 124, 20, 20,
                                                               unsetCircularCbk );

          /*** Use image as texture ***/
    gtk3sculptedit->circularToggle = ui_createToggleLabel    ( fixed, 
                                                               gtk3sculptedit,
                                                               EDITSCULPTTOOLCIRCULAR,
                                                               CLASS_MAIN,
                                                               0, 48, 124, 20, 20,
                                                               setCircularCbk );

    gtk3sculptedit->pressureScale = ui_createHorizontalScale ( fixed, 
                                                               gtk3sculptedit,
                                                               EDITSCULPTTOOLPRESSURE,
                                                               CLASS_MAIN,
                                                               0,  72, 124, 180,  32,
                                                               0.0f, 100.0f, 1.0f,
                                                               setPressureCbk );

    gtk3sculptedit->radiusScale =   ui_createHorizontalScale ( fixed, 
                                                               gtk3sculptedit,
                                                               EDITSCULPTTOOLRADIUS, 
                                                               CLASS_MAIN,
                                                               0,  96, 124, 180,  32,
                                                               0.0f, 100.0f, 1.0f, 
                                                               setRadiusCbk   );

    gtk_widget_show ( fixed );


    return gtk3sculptedit;
}

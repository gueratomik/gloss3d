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
typedef struct _G3DUISCULPTTOOLEDIT {
    G3DUIWIDGETGROUP grp;

    GtkWidget       *main;
    GtkWidget       *visibleToggle;
    GtkWidget       *radiusScale;
    GtkWidget       *pressureScale;

    G3DLIGHT        *editedLight;
} G3DUISCULPTTOOLEDIT;

/******************************************************************************/
static G3DUISCULPTTOOLEDIT *g3duisculptooledit_new ( G3DUI *gui ) {
    G3DUISCULPTTOOLEDIT *sed = calloc ( 0x01, sizeof ( G3DUISCULPTTOOLEDIT ) );

    if ( sed == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    sed->grp.gui = gui;

    return sed; 
}

/******************************************************************************/
static void setRadiusCbk  ( GtkWidget *widget, gpointer user_data ) {
    int radius = ( int ) gtk_range_get_value ( GTK_RANGE(widget) );
    G3DUI *gui    = ( G3DUI * ) user_data;

    common_g3duisculpttooledit_setRadiusCbk ( gui, radius );
}

/******************************************************************************/
static void setPressureCbk  ( GtkWidget *widget, gpointer user_data ) {
    float pressure = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );
    G3DUI *gui     = ( G3DUI * ) user_data;

    common_g3duisculpttooledit_setPressureCbk ( gui, pressure / 100.0f );
}

/******************************************************************************/
static void onlyVisibleCbk  ( GtkWidget *widget, gpointer user_data ) {
    int visible_only = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    G3DUI *gui    = ( G3DUI * ) user_data;

    common_g3duisculpttooledit_onlyVisibleCbk ( gui, visible_only );
}

/******************************************************************************/
static void setCircularCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duisculpttooledit_setCircularCbk ( gui );
}

/******************************************************************************/
static void unsetCircularCbk ( GtkWidget *widget, 
                               gpointer   user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duisculpttooledit_unsetCircularCbk ( gui );
}

/******************************************************************************/
void updateSculptToolEdit ( GtkWidget    *widget, 
                            G3DMOUSETOOL *tool ) {
    /*G3DMOUSETOOL *tool = common_g3dui_getMouseTool ( gui, INFLATETOOL );*/
    G3DMOUSETOOLSCULPT *sc = ( G3DMOUSETOOLSCULPT * ) tool;
    G3DUISCULPTTOOLEDIT *sed = ( G3DUISCULPTTOOLEDIT * ) 
                                    g_object_get_data ( G_OBJECT(widget),
                                                        "private" );

    /*** prevent a loop ***/
    sed->grp.gui->lock = 0x01;

    gtk_widget_set_sensitive ( sed->visibleToggle, FALSE );
    gtk_widget_set_sensitive ( sed->radiusScale  , FALSE );
    gtk_widget_set_sensitive ( sed->pressureScale, FALSE );

    if ( sc ) {
        gtk_widget_set_sensitive ( sed->visibleToggle, TRUE );
        gtk_widget_set_sensitive ( sed->radiusScale  , TRUE );
        gtk_widget_set_sensitive ( sed->pressureScale, TRUE );

        if ( sc->only_visible ) {
            gtk_toggle_button_set_active ( sed->visibleToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( sed->visibleToggle, FALSE );
        }

        gtk_range_set_value ( sed->radiusScale  , sc->radius );
        gtk_range_set_value ( sed->pressureScale, sc->pressure * 100.0f );

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

    sed->grp.gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUISCULPTTOOLEDIT *sed = ( G3DUISCULPTTOOLEDIT * ) user_data;

    free ( sed );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUISCULPTTOOLEDIT *sed = ( G3DUISCULPTTOOLEDIT * ) user_data;
    G3DUI *gui = sed->grp.gui;

    updateSculptToolEdit ( widget, gui->mou );
}

/******************************************************************************/
GtkWidget *createSculptToolEdit ( GtkWidget *parent, G3DUI *gui,
                                                     char *name,
                                                     gint x,
                                                     gint y,
                                                     gint width,
                                                     gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *frm, *ptf, *fix, *btn;
    G3DUISCULPTTOOLEDIT *sed = g3duisculptooledit_new ( gui );


    frm = gtk_fixed_new ( );

    g_object_set_data ( G_OBJECT(frm), "private", (gpointer) sed );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), sed );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), sed );
 
    sed->visibleToggle = createToggleLabel ( frm,
                                             gui,
                                             EDITSCULPTTOOLVISIBLE,
                                             0,   0,  96,  24,
                                             onlyVisibleCbk  );

          /*** Use image as texture ***/
    btn = createRadioLabel ( frm, gui,
                                  EDITSCULPTTOOLSQUARED,
                                  btn,
                                     0, 24, 96, 18,
                                   unsetCircularCbk );

          /*** Use image as texture ***/
          createRadioLabel ( frm, gui,
                                   EDITSCULPTTOOLCIRCULAR,
                                   btn,
                                     0, 48, 96, 18,
                                   setCircularCbk );

    createSimpleLabel ( frm, 
                        gui,
                        EDITSCULPTTOOLPRESSURE,
                        0, 
                        72,
                        48,
                        24 );

    sed->pressureScale = createHorizontalScale ( frm, 
                                                 gui,
                                                 EDITSCULPTTOOLPRESSURE, 
                                                 64,  72, 224,  32,
                                                 0.0f, 100.0f, 1.0f,
                                                 setPressureCbk );

    createSimpleLabel ( frm, 
                        gui,
                        EDITSCULPTTOOLRADIUS,
                        0, 
                        96,
                        48,
                        24 );

    sed->radiusScale = createHorizontalScale ( frm, 
                                               gui,
                                               EDITSCULPTTOOLRADIUS, 
                                               64,  96, 224,  32,
                                               0.0f, 100.0f, 1.0f, 
                                               setRadiusCbk   );

    gtk_widget_show ( frm );


    return frm;
}

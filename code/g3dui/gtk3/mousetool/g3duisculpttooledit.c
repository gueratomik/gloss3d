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
void updateSculptToolEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DMOUSETOOL *tool = common_g3dui_getMouseTool ( gui, SCULPTTOOL );
    G3DMOUSETOOLSCULPT *sc = ( G3DMOUSETOOLSCULPT * ) tool;

    /*** prevent a loop ***/
    gui->lock = 0x01;

    if ( sc ) {
        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( GTK_IS_CHECK_BUTTON(child) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITSCULPTTOOLVISIBLE ) == 0x00 ) {
                    if ( sc->only_visible ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    } 
                }
            }

            if ( GTK_IS_SCALE(child) ) {
                GtkRange *ran = GTK_RANGE(child);

                if ( strcmp ( child_name, EDITSCULPTTOOLPRESSURE ) == 0x00 ) {
                    gtk_range_set_value ( ran, ( sc->pressure * 100.0f ) );
                }

                if ( strcmp ( child_name, EDITSCULPTTOOLRADIUS ) == 0x00 ) {
                    gtk_range_set_value ( ran, sc->radius );
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateSculptToolEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget *createSculptToolEdit ( GtkWidget *parent, G3DUI *gui,
                                                     char *name,
                                                     gint x,
                                                     gint y,
                                                     gint width,
                                                     gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *frm, *ptf, *fix;

    frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), gui );

 
    createToggleLabel     ( frm, gui, EDITSCULPTTOOLVISIBLE,
                                  0,   0,  96,  32, onlyVisibleCbk  );

    createHorizontalScale ( frm, gui, EDITSCULPTTOOLPRESSURE, 
                                  0,  32, 256,  32,
                                 0.0f, 100.0f, 1.0f, setPressureCbk );

    createHorizontalScale ( frm, gui, EDITSCULPTTOOLRADIUS, 
                                  0,  64, 256,  32,
                                 0.0f, 100.0f, 1.0f, setRadiusCbk   );

    gtk_widget_show ( frm );


    return frm;
}

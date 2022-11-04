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

#define EDITPENTOOLRADIUS      "Radius"
#define EDITPENTOOLPRESSURE    "Pressure"
#define EDITPENTOOLINCREMENTAL "Incremental"

/******************************************************************************/
static GTK3M3DUIPENTOOLEDIT *gtk3_m3duipentooledit_new ( GTK3M3DUI *gtk3mui ) {
    uint32_t memsize = sizeof ( GTK3M3DUIPENTOOLEDIT );
    GTK3M3DUIPENTOOLEDIT *gtk3penedit = calloc ( 0x01, memsize );

    if ( gtk3penedit == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3penedit->core.mui = ( M3DUI * ) gtk3mui;


    return gtk3penedit; 
}

/******************************************************************************/
void gtk3_m3duipentooledit_update ( GTK3M3DUIPENTOOLEDIT *gtk3penedit ) {
    M3DUI *mui = gtk3penedit->core.mui;
    G3DUI *gui = mui->gui;
    G3DUIMOUSETOOL *mou = g3dui_getMouseTool ( gui, PENTOOL );
    M3DSYSINFO *sysinfo = m3dsysinfo_get ( );

    gui->lock = 0x01;

    if ( mou ) {
        M3DMOUSETOOLPEN *mtp = ( M3DMOUSETOOLPEN * ) mou->tool;
        M3DBASEPEN *bp = mtp->ltool.obj;

        gtk_spin_button_set_value ( gtk3penedit->radiusEntry, 
                                    sysinfo->pattern->size );

        if ( mtp->incremental ) {
            gtk_toggle_button_set_active ( gtk3penedit->incrementalToggle, 
                                           TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3penedit->incrementalToggle,
                                           FALSE );
        }

        gtk_range_set_value ( GTK_RANGE(gtk3penedit->pressureScale), 
                              ( bp->pressure * 100.0f ) );
    }

    gui->lock = 0x00;

}

/******************************************************************************/
static void setIncrementalCbk  ( GtkWidget *widget, 
                                 gpointer   user_data ) {
    GTK3M3DUIPENTOOLEDIT *gtk3penedit = ( GTK3M3DUIPENTOOLEDIT * ) user_data;
    int incremental = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    if ( gtk3penedit->core.mui->gui->lock ) return;

    m3duipentooledit_setIncremental ( &gtk3penedit->core, incremental );
}

/******************************************************************************/
static void setRadiusCbk  ( GtkWidget *widget, 
                            gpointer   user_data ) {
    GTK3M3DUIPENTOOLEDIT *gtk3penedit = ( GTK3M3DUIPENTOOLEDIT * ) user_data;
    int radius = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( gtk3penedit->core.mui->gui->lock ) return;

    m3duipentooledit_setRadius ( &gtk3penedit->core, radius );
}

/******************************************************************************/
static void setPressureCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3M3DUIPENTOOLEDIT *gtk3penedit = ( GTK3M3DUIPENTOOLEDIT * ) user_data;
    float pressure = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );

    if ( gtk3penedit->core.mui->gui->lock ) return;

    m3duipentooledit_setPressure ( &gtk3penedit->core, pressure / 100.0f );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3M3DUIPENTOOLEDIT *gtk3penedit = ( GTK3M3DUIPENTOOLEDIT * ) user_data;

    free ( gtk3penedit );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3M3DUIPENTOOLEDIT *gtk3penedit = ( GTK3M3DUIPENTOOLEDIT * ) user_data;

    gtk3_m3duipentooledit_update ( gtk3penedit );
}

/******************************************************************************/
GTK3M3DUIPENTOOLEDIT *gtk3_m3duipentooledit_create ( GtkWidget *parent, 
                                                     GTK3M3DUI *gtk3mui,
                                                     char      *name ) {
    GTK3M3DUIPENTOOLEDIT *gtk3penedit = gtk3_m3duipentooledit_new ( gtk3mui );
    GtkFixed *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk3penedit->fixed = fixed;

    gtk3penedit->pressureScale     = ui_createHorizontalScale ( fixed,
                                                                gtk3penedit,
                                                                EDITPENTOOLPRESSURE,
                                                                CLASS_MAIN,
                                                                0, 0, 96, 192, 20,
                                                                0.0f, 100.0f, 1.0f,
                                                                setPressureCbk );

    gtk3penedit->radiusEntry       = ui_createIntegerText     ( fixed,
                                                                gtk3penedit,
                                                                EDITPENTOOLRADIUS, 
                                                                CLASS_MAIN,
                                                                0, 255,
                                                                0, 24, 96, 96, 20, 
                                                                setRadiusCbk );

    gtk3penedit->incrementalToggle = ui_createToggleLabel     ( fixed,
                                                                gtk3penedit,
                                                                EDITPENTOOLINCREMENTAL,
                                                                CLASS_MAIN,
                                                                0, 48, 96, 96, 20,
                                                                setIncrementalCbk );

    g_signal_connect ( G_OBJECT (gtk3penedit->fixed), "realize", G_CALLBACK (Realize), gtk3penedit );
    g_signal_connect ( G_OBJECT (gtk3penedit->fixed), "destroy", G_CALLBACK (Destroy), gtk3penedit );

    gtk_widget_show ( gtk3penedit->fixed );


    return gtk3penedit;
}

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
static void setIncrementalCbk  ( GtkWidget *widget, 
                                 gpointer   user_data ) {
    int incremental = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    M3DUI *mui = ( M3DUI * ) user_data;

    common_g3duipentooledit_setIncrementalCbk ( mui, incremental );
}

/******************************************************************************/
static void setRadiusCbk  ( GtkWidget *widget, 
                            gpointer   user_data ) {
    int radius = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    M3DUI *mui = ( M3DUI * ) user_data;

    common_g3duipentooledit_setRadiusCbk ( mui, radius );
}

/******************************************************************************/
static void setPressureCbk  ( GtkWidget *widget, gpointer user_data ) {
    float pressure = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );
    M3DUI *mui = ( M3DUI * ) user_data;

    common_g3duipentooledit_setPressureCbk ( mui, pressure / 100.0f );
}

/******************************************************************************/
void updatePenToolEdit ( GtkWidget        *widget,
                         M3DUI *mui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    M3DMOUSETOOL *tool = common_g3dui_getMouseTool ( mui->gui, PENTOOL );
    M3DMOUSETOOLPEN *pen = ( M3DMOUSETOOLPEN * ) tool;
    M3DSYSINFO *sysinfo = m3dsysinfo_get ( );

    /*** prevent a loop ***/
    mui->gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( GTK_IS_SPIN_BUTTON(child) ) {
            GtkSpinButton *spb = GTK_SPIN_BUTTON(child);

            if ( strcmp ( child_name, EDITPENTOOLRADIUS   ) == 0x00 ) {
                gtk_spin_button_set_value ( spb, sysinfo->pattern->size );
            }
        }

        if ( GTK_IS_CHECK_BUTTON(child) ) {
            GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

            if ( strcmp ( child_name, EDITPENTOOLINCREMENTAL ) == 0x00 ) {
                if ( pen->incremental ) {
                    gtk_toggle_button_set_active ( tbn, TRUE  );
                } else {
                    gtk_toggle_button_set_active ( tbn, FALSE );
                }
            }
        }

        if ( GTK_IS_SCALE (child) ) {
            GtkRange *ran = GTK_RANGE(child);

            if ( strcmp ( child_name, EDITPENTOOLPRESSURE ) == 0x00 ) {
                M3DBASEPEN *bpobj = ( M3DBASEPEN * ) tool->obj;

                gtk_range_set_value ( ran, ( bpobj->pressure * 100.0f ) );
            }
        }

        children =  g_list_next ( children );
    }

    mui->gui->lock = 0x00;
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    M3DUI *mui = ( M3DUI * ) user_data;

    updatePenToolEdit ( widget, mui );
}

/******************************************************************************/
GtkWidget *createPenToolEdit ( GtkWidget        *parent, 
                               M3DUI *mui,
                               char             *name,
                               gint              x,
                               gint              y,
                               gint              width,
                               gint              height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *frm, *ptf, *fix;

    frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), mui );


    createSimpleLabel ( frm, mui, EDITPENTOOLPRESSURE,
                        0x00, 
                        0x00, 
                        0x60,
                        24 );
    createHorizontalScale ( frm, mui, EDITPENTOOLPRESSURE,
                                 96,   0, 160,  24,
                                 0.0f, 100.0f, 1.0f, setPressureCbk );

    createIntegerText     ( frm, mui, EDITPENTOOLRADIUS, 
                                  0, 256,
                                  0, 24, 96,  32, setRadiusCbk );

    createToggleLabel     ( frm, mui, EDITPENTOOLINCREMENTAL,
                                  0, 48, 96,  24, setIncrementalCbk );


    gtk_widget_show ( frm );


    return frm;
}

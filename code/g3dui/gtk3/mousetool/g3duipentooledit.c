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

#define EDITPENTOOLRADIUS   "Radius"
#define EDITPENTOOLPRESSURE "Pressure"

/******************************************************************************/
static void setRadiusCbk  ( GtkWidget *widget, 
                            gpointer   user_data ) {
    int radius = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUIUVMAPEDITOR *uvme = ( G3DUIUVMAPEDITOR * ) user_data;

    common_g3duipentooledit_setRadiusCbk ( uvme, radius );
}

/******************************************************************************/
static void setPressureCbk  ( GtkWidget *widget, gpointer user_data ) {
    float pressure = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );
    G3DUIUVMAPEDITOR *uvme = ( G3DUIUVMAPEDITOR * ) user_data;

    common_g3duipentooledit_setPressureCbk ( uvme, pressure / 100.0f );
}

/******************************************************************************/
void updatePenToolEdit ( GtkWidget        *widget,
                         G3DUIUVMAPEDITOR *uvme ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    L3DMOUSETOOL *tool = common_g3dui_getMouseTool ( uvme->gui, PENTOOL );
    L3DMOUSETOOLPEN *pen = ( L3DMOUSETOOLPEN * ) tool;
    L3DSYSINFO *sysinfo = l3dsysinfo_get ( );

    /*** prevent a loop ***/
    uvme->gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( GTK_IS_SPIN_BUTTON(child) ) {
            GtkSpinButton *spb = GTK_SPIN_BUTTON(child);

            if ( strcmp ( child_name, EDITPENTOOLRADIUS   ) == 0x00 ) {
                gtk_spin_button_set_value ( spb, sysinfo->pattern->size );
            }
        }

        if ( GTK_IS_SCALE (child) ) {
            GtkRange *ran = GTK_RANGE(child);

            if ( strcmp ( child_name, EDITPENTOOLPRESSURE ) == 0x00 ) {
                gtk_range_set_value ( ran, ( tool->obj->pressure * 100.0f ) );
            }
        }

        children =  g_list_next ( children );
    }

    uvme->gui->lock = 0x00;
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUIUVMAPEDITOR *uvme = ( G3DUIUVMAPEDITOR * ) user_data;

    updatePenToolEdit ( widget, uvme );
}

/******************************************************************************/
GtkWidget *createPenToolEdit ( GtkWidget        *parent, 
                               G3DUIUVMAPEDITOR *uvme,
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

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), uvme );

    createHorizontalScale ( frm, uvme, EDITPENTOOLPRESSURE,
                                  0,   0, 256,  32,
                                 0.0f, 100.0f, 1.0f, setPressureCbk );

    createIntegerText     ( frm, uvme, EDITPENTOOLRADIUS, 
                                  0, 256,
                                  0, 32,   96,  32, setRadiusCbk );


    gtk_widget_show ( frm );


    return frm;
}

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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
static void radiusCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duisphereedit_radiusCbk ( gui, rad );
}

/******************************************************************************/
static void capCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int cap = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( cap >= 0x02 ) {
        common_g3duisphereedit_capCbk ( gui, cap );
    } else {
        updateSphereEdit ( parent, gui );
    }
}

/******************************************************************************/
static void sliceCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int slice = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( slice >= 0x03 ) {
        common_g3duisphereedit_sliceCbk ( gui, slice );
    } else {
        updateSphereEdit ( parent, gui );
    }
}

/******************************************************************************/
void updateSphereEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DSPHERETYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        SPHEREDATASTRUCT *sds = ( SPHEREDATASTRUCT * ) pri->data;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *name = gtk_widget_get_name ( child );

            if ( GTK_IS_SPIN_BUTTON ( child ) ) {
                GtkSpinButton *spb = GTK_SPIN_BUTTON ( child );

                if ( strcmp ( name, EDITSPHERECAPS   ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, sds->cap );
                }

                if ( strcmp ( name, EDITSPHERESLICES ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, sds->slice );
                }

                if ( strcmp ( name, EDITSPHERERADIUS ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, sds->radius );
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
GtkWidget *createSphereEdit ( GtkWidget *parent, G3DUI *gui, 
                                                 char *name,
                                                 gint x, 
                                                 gint y,
                                                 gint width,
                                                 gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget * frm;

    frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );

    createIntegerText ( frm, gui, EDITSPHERECAPS  , 0, 0,
                                                    96, 32, capCbk    );

    createIntegerText ( frm, gui, EDITSPHERESLICES, 0, 20, 
                                                    96, 32, sliceCbk  );

    createFloatText   ( frm, gui, EDITSPHERERADIUS, 0, 40,
                                                    96, 64, radiusCbk );

    gtk_widget_show ( frm );


    return frm;
}

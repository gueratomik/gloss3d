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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
static void xsliceCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int slice = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( slice ) {
        common_g3duicubeedit_sliceCbk ( gui, G3DUIXAXIS, slice );
    } else {
        updateCubeEdit ( parent, gui );
    }
}

/******************************************************************************/
static void ysliceCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int slice = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( slice ) {
        common_g3duicubeedit_sliceCbk ( gui, G3DUIYAXIS, slice );
    } else {
        updateCubeEdit ( parent, gui );
    }
}

/******************************************************************************/
static void zsliceCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int slice = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( slice ) {
        common_g3duicubeedit_sliceCbk ( gui, G3DUIZAXIS, slice );
    } else {
        updateCubeEdit ( parent, gui );
    }
}

/******************************************************************************/
static void radiusCbk ( GtkWidget *widget, gpointer user_data ) {
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duicubeedit_radiusCbk ( gui, rad );
}

/******************************************************************************/
void updateCubeEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DCUBETYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        CUBEDATASTRUCT *cds = ( CUBEDATASTRUCT * ) pri->data;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *name = gtk_widget_get_name ( child );
            const char *type = G_OBJECT_TYPE_NAME ( child );

            if ( strcmp ( type, "GtkSpinButton" ) == 0x00 ) {
                GtkSpinButton *spb = GTK_SPIN_BUTTON(child);

                if ( strcmp ( name, EDITCUBEXSLICES   ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, cds->nbx );
                }

                if ( strcmp ( name, EDITCUBEYSLICES ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, cds->nby );
                }

                if ( strcmp ( name, EDITCUBEZSLICES ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, cds->nbz );
                }

                if ( strcmp ( name, EDITCUBERADIUS ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, cds->radius );
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}


/******************************************************************************/
GtkWidget *createCubeEdit ( GtkWidget *parent, G3DUI *gui, 
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

    createIntegerText ( frm, gui, EDITCUBEXSLICES, 0x00, 0x00, 
                                                   0x60, 0x20, xsliceCbk );

    createIntegerText ( frm, gui, EDITCUBEYSLICES, 0x00, 0x14, 
                                                   0x60, 0x20, ysliceCbk );

    createIntegerText ( frm, gui, EDITCUBEZSLICES, 0x00, 0x28, 
                                                   0x60, 0x20, zsliceCbk );

    createFloatText   ( frm, gui, EDITCUBERADIUS , 0x00, 0x3C, 
                                                   0x60, 0x60, radiusCbk );

    gtk_widget_show ( frm );


    return frm;
}

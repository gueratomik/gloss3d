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
static void lengthCbk ( GtkWidget *widget, gpointer user_data ) {
    float len = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duitubeedit_lengthCbk ( gui, len );
}

/******************************************************************************/
static void radiusCbk ( GtkWidget *widget, gpointer user_data ) {
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duitubeedit_radiusCbk ( gui, rad );
}

/******************************************************************************/
static void thicknessCbk ( GtkWidget *widget, gpointer user_data ) {
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duitubeedit_thicknessCbk ( gui, rad );
}

/******************************************************************************/
static void xcapCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int cap = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( cap ) {
        common_g3duitubeedit_capCbk ( gui, G3DUIXAXIS, cap );
    } else {
        updateTubeEdit ( parent, gui );
    }
}

/******************************************************************************/
static void ycapCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int cap = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( cap ) {
        common_g3duitubeedit_capCbk ( gui, G3DUIYAXIS, cap );
    } else {
        updateTubeEdit ( parent, gui );
    }
}

/******************************************************************************/
static void sliceCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int slice = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( slice >= 0x03 ) {
        common_g3duitubeedit_sliceCbk ( gui, slice );
    } else {
        updateTubeEdit ( parent, gui );
    }
}

/******************************************************************************/
void updateTubeEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DTUBETYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        TUBEDATASTRUCT *cds = ( TUBEDATASTRUCT * ) pri->data;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *name = gtk_widget_get_name ( child );
            const char *type = G_OBJECT_TYPE_NAME ( child );

            if ( strcmp ( type, "GtkSpinButton" ) == 0x00 ) {
                GtkSpinButton *spb = GTK_SPIN_BUTTON(child);

                if ( strcmp ( name, EDITTUBEXCAPS   ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, cds->capx );
                }

                if ( strcmp ( name, EDITTUBEYCAPS   ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, cds->capy );
                }

                if ( strcmp ( name, EDITTUBESLICES ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, cds->slice );
                }

                if ( strcmp ( name, EDITTUBERADIUS ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, cds->radius );
                }

                if ( strcmp ( name, EDITTUBETHICKNESS ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, cds->thickness );
                }

                if ( strcmp ( name, EDITTUBELENGTH ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, cds->length );
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
GtkWidget *createTubeEdit ( GtkWidget *parent, G3DUI *gui, 
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

    createIntegerText ( frm, gui, EDITTUBEXCAPS , 1, 100,
                                                      0x00, 0,
                                                      0x60, 0x20, xcapCbk   );

    /*createIntegerText ( frm, EDITTUBEYCAPS , 0, 20, 32, capycbk );*/
    createIntegerText ( frm, gui, EDITTUBESLICES, 1, 100,
                                                      0x00, 24, 
                                                      0x60, 0x20, sliceCbk  );

    createFloatText   ( frm, gui, EDITTUBELENGTH, 0.0f, FLT_MAX,
                                                      0x00, 48, 
                                                      0x60, 0x60, lengthCbk );

    createFloatText   ( frm, gui, EDITTUBERADIUS, 0.0f, FLT_MAX,
                                                      0x00, 72, 
                                                      0x60, 0x60, radiusCbk );

    createFloatText   ( frm, gui, EDITTUBETHICKNESS, 0.0f, FLT_MAX,
                                                      0x00, 96, 
                                                      0x60, 0x60, thicknessCbk );

    gtk_widget_show ( frm );


    return frm;
}

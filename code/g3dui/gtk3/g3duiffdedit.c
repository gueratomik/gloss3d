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
        common_g3duiffdedit_sliceCbk ( gui, G3DUIXAXIS, slice );
    } else {
        updateFFDEdit ( parent, gui );
    }
    g3dui_redrawObjectList ( gui );
}

/******************************************************************************/
static void ysliceCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int slice = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( slice ) {
        common_g3duiffdedit_sliceCbk ( gui, G3DUIYAXIS, slice );
    } else {
        updateFFDEdit ( parent, gui );
    }
    g3dui_redrawObjectList ( gui );
}

/******************************************************************************/
static void zsliceCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int slice = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( slice ) {
        common_g3duiffdedit_sliceCbk ( gui, G3DUIZAXIS, slice );
    } else {
        updateFFDEdit ( parent, gui );
    }
    g3dui_redrawObjectList ( gui );
}

/******************************************************************************/
static void xradiusCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( rad >= 0.0f ) {
        common_g3duiffdedit_radiusCbk ( gui, G3DUIXAXIS, rad );
    } else {
        updateFFDEdit ( parent, gui );
    }
    g3dui_redrawObjectList ( gui );
}

/******************************************************************************/
static void yradiusCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( rad >= 0.0f ) {
        common_g3duiffdedit_radiusCbk ( gui, G3DUIYAXIS, rad );
    } else {
        updateFFDEdit ( parent, gui );
    }
    g3dui_redrawObjectList ( gui );
}

/******************************************************************************/
static void zradiusCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( rad >= 0.0f ) {
        common_g3duiffdedit_radiusCbk ( gui, G3DUIZAXIS, rad );
    } else {
        updateFFDEdit ( parent, gui );
    }
    g3dui_redrawObjectList ( gui );
}

/******************************************************************************/
void updateFFDEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent a loop ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DFFDTYPE ) ) {
        G3DFFD *ffd = ( G3DFFD * ) obj;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( GTK_IS_SPIN_BUTTON ( child ) ) {
                GtkSpinButton *spb = GTK_SPIN_BUTTON ( child );

                if ( strcmp ( child_name, EDITFFDXSLICES   ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, ffd->nbx );
                }

                if ( strcmp ( child_name, EDITFFDYSLICES ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, ffd->nby );
                }

                if ( strcmp ( child_name, EDITFFDZSLICES ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, ffd->nbz );
                }

                if ( strcmp ( child_name, EDITFFDXRADIUS ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, ffd->locmax.x );
                }

                if ( strcmp ( child_name, EDITFFDYRADIUS ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, ffd->locmax.y );
                }

                if ( strcmp ( child_name, EDITFFDZRADIUS ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, ffd->locmax.z );
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

    updateFFDEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget *createFFDEdit ( GtkWidget *parent, G3DUI *gui,
                                              char *name,
                                              gint x,
                                              gint y,
                                              gint width,
                                              gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *frm, *sdf, *fix;

    frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), gui );

    createIntegerText ( frm, gui, EDITFFDXSLICES, 1, 100,        0,   0, 96, 32, xsliceCbk  );
    createIntegerText ( frm, gui, EDITFFDYSLICES, 1, 100,        0,  24, 96, 32, ysliceCbk  );
    createIntegerText ( frm, gui, EDITFFDZSLICES, 1, 100,        0,  48, 96, 32, zsliceCbk  );
    createFloatText   ( frm, gui, EDITFFDXRADIUS, 0.0f, FLT_MAX, 0,  72, 96, 32, xradiusCbk );
    createFloatText   ( frm, gui, EDITFFDYRADIUS, 0.0f, FLT_MAX, 0,  96, 96, 32, yradiusCbk );
    createFloatText   ( frm, gui, EDITFFDZRADIUS, 0.0f, FLT_MAX, 0, 120, 96, 32, zradiusCbk );

    gtk_widget_show ( frm );


    return frm;
}

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
static void limitCbk ( GtkWidget *widget, gpointer user_data ) {
    float lim = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duisymmetryedit_limitCbk ( gui, lim );
}

/******************************************************************************/
static void planeCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkComboBoxText *cmbt = GTK_COMBO_BOX_TEXT(widget);
    char *orientation = gtk_combo_box_text_get_active_text ( cmbt );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duisymmetryedit_planeCbk ( gui, orientation );
}

/******************************************************************************/
void updateSymmetryEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent a loop ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DSYMMETRYTYPE ) ) {
        G3DSYMMETRY *sym = ( G3DSYMMETRY * ) obj;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( GTK_IS_COMBO_BOX_TEXT(child) ) {
                GtkComboBox *cmb = GTK_COMBO_BOX(child);

                if ( strcmp ( child_name, EDITSYMMETRYPLANE   ) == 0x00 ) {
                    gtk_combo_box_set_active ( cmb, sym->orientation );
                }
            }

            if ( GTK_IS_SPIN_BUTTON(child) ) {
                GtkSpinButton *spb = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITSYMMETRYLIMIT   ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, sym->mergelimit );
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateSymmetryEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget *createSymmetryEdit ( GtkWidget *parent, G3DUI *gui,
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

    createFloatText            ( frm, gui, EDITSYMMETRYLIMIT, 0,  0,
                                                             96, 96, limitCbk );
    createOrientationSelection ( frm, gui, EDITSYMMETRYPLANE, 0, 32,
                                                             96, 32, planeCbk );


    gtk_widget_show ( frm );


    return frm;
}

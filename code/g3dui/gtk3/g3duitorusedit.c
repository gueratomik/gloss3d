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
static void sliceCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    int slice = gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    if ( slice >= 0x03 ) {
        common_g3duitorusedit_sliceCbk ( gui, slice );
    } else {
        updateTorusEdit ( parent, gui );
    }
}

/******************************************************************************/
static void capCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    int cap = gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    if ( cap >= 0x03 ) {
        common_g3duitorusedit_sliceCbk ( gui, cap );
    } else {
        updateTorusEdit ( parent, gui );
    }
}

/******************************************************************************/
static void extRadiusCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float rad = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    common_g3duitorusedit_extRadiusCbk ( gui, rad );
}

/******************************************************************************/
static void intRadiusCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float rad = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    common_g3duitorusedit_intRadiusCbk ( gui, rad );
}

/******************************************************************************/
static void orientationCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkComboBoxText *cmbt = GTK_COMBO_BOX_TEXT(widget);
    G3DUI *gui = ( G3DUI * ) user_data;
    const char *str = gtk_combo_box_text_get_active_text ( cmbt );

    common_g3duitorusedit_orientationCbk ( gui, str );
}

/******************************************************************************/
void updateTorusEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DTORUSTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *name = gtk_widget_get_name ( child );

            if ( GTK_IS_SPIN_BUTTON(child) ) {
                GtkSpinButton *sbn = GTK_SPIN_BUTTON(child);

                if ( strcmp ( name, EDITTORUSSLICE   ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, tds->slice );
                }

                if ( strcmp ( name, EDITTORUSCAP ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, tds->cap );
                }

                if ( strcmp ( name, EDITTORUSEXTRAD ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, tds->extrad );
                }

                if ( strcmp ( name, EDITTORUSINTRAD ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, tds->intrad );
                }
            }

            if ( GTK_IS_COMBO_BOX_TEXT(child) ) {
                GtkComboBox *cmb = GTK_COMBO_BOX(child);

                if ( strcmp ( name, EDITTORUSORIENTATION   ) == 0x00 ) {
                    gtk_combo_box_set_active ( cmb, tds->orientation );
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
GtkWidget *createTorusEdit ( GtkWidget *parent, G3DUI *gui,
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

    createIntegerText ( frm, gui, EDITTORUSSLICE , 0x00, 0x00,
                                                   0x60, 0x20, sliceCbk  );

    createIntegerText ( frm, gui, EDITTORUSCAP   , 0x00, 0x14, 
                                                   0x60, 0x20, capCbk    );

    createFloatText   ( frm, gui, EDITTORUSEXTRAD, 0x00, 0x28,
                                                   0x60, 0x60, extRadiusCbk );

    createFloatText   ( frm, gui, EDITTORUSINTRAD, 0x00, 0x3C, 
                                                   0x60, 0x60, intRadiusCbk );

    createOrientationSelection ( frm, gui, EDITTORUSORIENTATION, 0x00, 0x50,
                                                                 0x60, 0x60, orientationCbk );

    gtk_widget_show ( frm );


    return frm;
}

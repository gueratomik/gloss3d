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
static void divuCbk  ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent     ( widget );
    G3DUI        *gui = ( G3DUI * ) user_data;
    int div = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( div ) {
        common_g3duiplaneedit_divCbk ( gui, G3DUIUAXIS, div );
    } else {
        updatePlaneEdit ( parent, gui );
    }
}

/******************************************************************************/
static void divvCbk  ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent     ( widget );
    G3DUI        *gui = ( G3DUI * ) user_data;
    int div = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( div ) {
        common_g3duiplaneedit_divCbk ( gui, G3DUIVAXIS, div );
    } else {
        updatePlaneEdit ( parent, gui );
    }
}

/******************************************************************************/
static void raduCbk  ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent     ( widget );
    G3DUI        *gui = ( G3DUI * ) user_data;
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( rad >= 0.0f ) {
        common_g3duiplaneedit_radiusCbk ( gui, G3DUIUAXIS, rad );
    } else {
        updatePlaneEdit ( parent, gui );
    }
}

/******************************************************************************/
static void radvCbk  ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent     ( widget );
    G3DUI        *gui = ( G3DUI * ) user_data;
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( rad >= 0.0f ) {
        common_g3duiplaneedit_radiusCbk ( gui, G3DUIVAXIS, rad );
    } else {
        updatePlaneEdit ( parent, gui );
    }
}

/******************************************************************************/
static void orientationCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkComboBoxText *cmbt = GTK_COMBO_BOX_TEXT(widget);
    GtkWidget  *parent = gtk_widget_get_parent     ( widget );
    G3DUI         *gui = ( G3DUI * ) user_data;
    const char *oristr = gtk_combo_box_text_get_active_text ( cmbt );

    if ( oristr ) {
        common_g3duiplaneedit_orientationcbk ( gui, oristr );
    } else {
        updatePlaneEdit ( parent, gui );
    }
}

/******************************************************************************/
void updatePlaneEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DPLANETYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        PLANEDATASTRUCT *pds = ( PLANEDATASTRUCT * ) pri->data;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *name = gtk_widget_get_name ( child );

            if ( GTK_IS_SPIN_BUTTON(child) ) {
                GtkSpinButton *spb = GTK_SPIN_BUTTON(child);

                if ( strcmp ( name, EDITPLANEDIVU   ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, pds->nbu );
                }

                if ( strcmp ( name, EDITPLANEDIVV ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, pds->nbv );
                }

                if ( strcmp ( name, EDITPLANERADU ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, pds->radu );
                }

                if ( strcmp ( name, EDITPLANERADV ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, pds->radv );
                }
            }

            if ( GTK_IS_COMBO_BOX_TEXT ( child ) ) {
                GtkComboBox *cmb = GTK_COMBO_BOX(child);

                if ( strcmp ( name, EDITPLANEORIENTATION   ) == 0x00 ) {
                    gtk_combo_box_set_active ( cmb, pds->orientation );
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
GtkWidget *createPlaneEdit ( GtkWidget *parent, G3DUI *gui,
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

    createIntegerText ( frm, gui, EDITPLANEDIVU, 0x00, 0x00,
                                                 0x60, 0x20, divuCbk );

    createIntegerText ( frm, gui, EDITPLANEDIVV, 0x00, 0x14, 
                                                 0x60, 0x20, divvCbk );

    createFloatText   ( frm, gui, EDITPLANERADU, 0x00, 0x28, 
                                                 0x60, 0x60, raduCbk );

    createFloatText   ( frm, gui, EDITPLANERADV, 0x00, 0x3C, 
                                                 0x60, 0x60, radvCbk );

    createOrientationSelection ( frm, gui, EDITPLANEORIENTATION, 0x00, 0x50, 
                                                                 0x60, 0x60, orientationCbk );

    gtk_widget_show ( frm );


    return frm;
}

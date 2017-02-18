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
void restrictCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(widget);
    int restricted = (int) gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(tbn));
    G3DUI *gui    = ( G3DUI * ) user_data;

    common_g3duicutmeshtooledit_restrictCbk ( gui, restricted );
}

/******************************************************************************/
void updateCutMeshToolEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DMOUSETOOL *mou = common_g3dui_getMouseTool ( gui, PICKTOOL );

    /*** prevent a loop ***/
    gui->lock = 0x01;

    if ( mou ) {
        G3DCUTMESH *cm = mou->data;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( GTK_IS_CHECK_BUTTON(child) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITCUTMESHTOOLRESTRICT ) == 0x00 ) {
                    if ( cm->restrict_to_selection ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    } 
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

    updateCutMeshToolEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget *createCutMeshToolEdit ( GtkWidget *parent, G3DUI *gui,
                                                      char *name,
                                                      gint x,
                                                      gint y,
                                                      gint width,
                                                      gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *frm, *ptf, *fix;

    frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), gui );

 
    createToggleLabel ( frm, gui, EDITCUTMESHTOOLRESTRICT, 0,  0, 
                                                          96, 32, restrictCbk );


    gtk_widget_show ( frm );


    return frm;
}

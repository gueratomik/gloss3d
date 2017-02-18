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
void updateBoneEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent a loop ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DBONETYPE ) ) {
        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( GTK_IS_SCROLLED_WINDOW (child) ) {
                if ( strcmp ( child_name, EDITBONEVERTEXGROUP ) == 0x00 ) {
                    GtkWidget *bin_child = gtk_bin_get_child ( GTK_BIN(child) );
                    /*updateBoneEditComboBox ( child, gui );*/
                    /*** If I redraw the ScrolledWindow, the weightgroup list**/
                    /*** does not get redrawn immediately, so we have to get **/
                    /*** the widget directly ***/ 
                    gtk_widget_queue_draw ( bin_child );
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
void g3duiboneedit_init ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateBoneEdit ( widget, gui );
}


/******************************************************************************/
GtkWidget *createBoneEdit ( GtkWidget *parent, G3DUI *gui,
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

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (g3duiboneedit_init), gui );

    createSimpleLabel ( frm, gui, EDITBONEVERTEXGROUP, 0,  0, 256, 20  );

    createBoneLinker  ( frm, gui, EDITBONEVERTEXGROUP, 0, 24, 218, 128 );

    /*createPushButton  ( frm, gui, "+", 246,  24, 32, 32, addvergrpcbk );
    createPushButton  ( frm, gui, "-", 246, 120, 32, 32, delvergrpcbk );*/

    gtk_widget_show ( frm );


    return frm;
}

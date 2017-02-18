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
static void nameCbk ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    const char *object_name = gtk_entry_get_text ( GTK_ENTRY(widget) );

    common_g3duiobjectedit_nameCbk ( gui, object_name );
}

/******************************************************************************/
void updateObjectEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj ) {
        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( GTK_IS_ENTRY(child) ) {
                GtkEntry *ent = GTK_ENTRY(child);

                if ( strcmp ( child_name, EDITOBJECTNAME ) == 0x00 ) {
                    gtk_entry_set_text ( ent, obj->name );
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
GtkWidget *createObjectEdit ( GtkWidget *parent, G3DUI *gui,
                                                 char *name,
                                                 gint x,
                                                 gint y,
                                                 gint width,
                                                 gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget * frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );


    createCharText ( frm, gui, EDITOBJECTNAME, 0, 0, 96, 96, nameCbk );


    gtk_widget_show ( frm );


    return frm;
}

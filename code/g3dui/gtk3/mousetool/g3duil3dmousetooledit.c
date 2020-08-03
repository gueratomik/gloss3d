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
void updateL3DMouseTool ( GtkWidget        *widget, 
                          G3DUIUVMAPEDITOR *uvme ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DMOUSETOOL *mou = uvme->gui->uvmou;

    if ( mou ) {
        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( strcmp ( child_name, mou->name ) == 0x00 ) {
                if ( strcmp ( mou->name, PENTOOL ) == 0x00 ) updatePenToolEdit ( child, uvme );

                gtk_widget_show ( child );
            } else {
                gtk_widget_hide ( child );
            }

            children =  g_list_next ( children );
        }
    /*** Hide all children widgets if no object is selected ***/
    } else {

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;

            gtk_widget_hide ( child );

            children =  g_list_next ( children );
        }
    }
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUIUVMAPEDITOR *uvme = ( G3DUIUVMAPEDITOR * ) user_data;

    updateL3DMouseTool ( widget, uvme );
}

/******************************************************************************/
GtkWidget *createL3DMouseToolEdit ( GtkWidget        *parent, 
                                    G3DUIUVMAPEDITOR *uvme,
                                    char             *name,
                                    gint              x,
                                    gint              y,
                                    gint              width,
                                    gint              height ) {
    G3DUI *gui = uvme->gui;
    GdkRectangle scrrec = { 0, 0, width, height };
    GdkRectangle frmrec = { 0, 0, 256, 256 };
    GtkWidget *label = gtk_label_new ( name );
    GtkWidget *scr, *frm;

    scr = gtk_scrolled_window_new ( NULL, NULL );

    gtk_widget_set_name ( scr, name );

    gtk_widget_set_size_request ( scr, scrrec.width, scrrec.height );

    gtk_fixed_put ( parent, scr, x, y );

    frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &frmrec );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_container_add ( GTK_CONTAINER(scr), frm );
#else
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(scr), frm );
#endif

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), uvme );

    /*** This is type dependent: hidden if not of ***/
    /*** selected object type showed otherwise.   ***/
    createPenToolEdit ( frm, uvme, PENTOOL, 0, 0, 256, 192 );

    list_insert ( &uvme->lmtools, frm );

    gtk_widget_show ( frm );
    gtk_widget_show ( scr );


    return scr;
}

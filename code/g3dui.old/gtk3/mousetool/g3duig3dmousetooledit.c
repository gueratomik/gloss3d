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
void updateCurrentMouseTool ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DMOUSETOOL *mou = gui->mou;

    if ( mou ) {
        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( strcmp ( child_name, mou->name ) == 0x00 ) {
                if ( strcmp ( mou->name, PICKTOOL     ) == 0x00 ) updatePickToolEdit    ( child, mou );
                if ( strcmp ( mou->name, MOVETOOL     ) == 0x00 ) updatePickToolEdit    ( child, mou );
                if ( strcmp ( mou->name, ROTATETOOL   ) == 0x00 ) updatePickToolEdit    ( child, mou );
                if ( strcmp ( mou->name, SCALETOOL    ) == 0x00 ) updatePickToolEdit    ( child, mou );
                if ( strcmp ( mou->name, PICKUVTOOL   ) == 0x00 ) updatePickToolEdit    ( child, mou );
                if ( strcmp ( mou->name, MOVEUVTOOL   ) == 0x00 ) updatePickToolEdit    ( child, mou );
                if ( strcmp ( mou->name, ROTATEUVTOOL ) == 0x00 ) updatePickToolEdit    ( child, mou );
                if ( strcmp ( mou->name, SCALEUVTOOL  ) == 0x00 ) updatePickToolEdit    ( child, mou );
                if ( strcmp ( mou->name, CUTMESHTOOL  ) == 0x00 ) updateCutMeshToolEdit ( child, gui );
                if ( strcmp ( mou->name, INFLATETOOL  ) == 0x00 ) updateSculptToolEdit  ( child, mou );
                if ( strcmp ( mou->name, SMOOTHTOOL   ) == 0x00 ) updateSculptToolEdit  ( child, mou );
                if ( strcmp ( mou->name, CREASETOOL   ) == 0x00 ) updateSculptToolEdit  ( child, mou );
                if ( strcmp ( mou->name, FLATTENTOOL  ) == 0x00 ) updateSculptToolEdit  ( child, mou );
                if ( strcmp ( mou->name, UNSCULPTTOOL ) == 0x00 ) updateSculptToolEdit  ( child, mou );

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
    G3DUI *gui = ( G3DUI * ) user_data;

    updateCurrentMouseTool ( widget, gui );
}

/******************************************************************************/
GtkWidget *createG3DMouseToolEdit ( GtkWidget *parent, 
                                    G3DUI     *gui,
                                    char *name,
                                    gint x,
                                    gint y,
                                    gint width,
                                    gint height ) {
    GdkRectangle scrrec = { 0, 0, width, height };
    GdkRectangle frmrec = { 0, 0, 320, 256 };
    GtkWidget *label = gtk_label_new ( name );
    GtkWidget *scr, *frm;

    scr = gtk_scrolled_window_new ( NULL, NULL );

    gtk_widget_set_name ( scr, name );

    gtk_widget_set_size_request ( scr, scrrec.width, scrrec.height );

    gtk_notebook_append_page ( GTK_NOTEBOOK(parent), scr, label );

    /*gtk_fixed_put ( parent, scr, x, y );*/



    frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &frmrec );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_container_add ( GTK_CONTAINER(scr), frm );
#else
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(scr), frm );
#endif

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), gui );

    /*** This is type dependent: hidden if not of ***/
    /*** selected object type showed otherwise.   ***/
    createPickToolEdit    ( frm, gui, PICKTOOL    , 0, 0, 320, 192 );
    createPickToolEdit    ( frm, gui, MOVETOOL    , 0, 0, 320, 192 );
    createPickToolEdit    ( frm, gui, ROTATETOOL  , 0, 0, 320, 192 );
    createPickToolEdit    ( frm, gui, SCALETOOL   , 0, 0, 320, 192 );
    createPickToolEdit    ( frm, gui, PICKUVTOOL  , 0, 0, 320, 192 );
    createPickToolEdit    ( frm, gui, MOVEUVTOOL  , 0, 0, 320, 192 );
    createPickToolEdit    ( frm, gui, ROTATEUVTOOL, 0, 0, 320, 192 );
    createPickToolEdit    ( frm, gui, SCALEUVTOOL , 0, 0, 320, 192 );
    createCutMeshToolEdit ( frm, gui, CUTMESHTOOL , 0, 0, 320, 320 );
    createSculptToolEdit  ( frm, gui, INFLATETOOL , 0, 0, 320, 192 );
    createSculptToolEdit  ( frm, gui, SMOOTHTOOL  , 0, 0, 320, 192 );
    createSculptToolEdit  ( frm, gui, CREASETOOL  , 0, 0, 320, 192 );
    createSculptToolEdit  ( frm, gui, FLATTENTOOL , 0, 0, 320, 192 );
    createSculptToolEdit  ( frm, gui, UNSCULPTTOOL, 0, 0, 320, 192 );

    list_insert ( &gui->lmtools, frm );

    gtk_widget_show ( frm );
    gtk_widget_show ( scr );


    return scr;
}

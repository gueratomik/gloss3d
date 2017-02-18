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
void updateCurrentEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj ) {
        char *wname = NULL;

        if ( obj->type == G3DSPHERETYPE     ) wname = EDITSPHERE;
        if ( obj->type == G3DCUBETYPE       ) wname = EDITCUBE;
        if ( obj->type == G3DCYLINDERTYPE   ) wname = EDITCYLINDER;
        if ( obj->type == G3DMESHTYPE       ) wname = EDITMESH;
        if ( obj->type == G3DLIGHTTYPE      ) wname = EDITLIGHT;
        if ( obj->type == G3DSYMMETRYTYPE   ) wname = EDITSYMMETRY;
        if ( obj->type == G3DFFDTYPE        ) wname = EDITFFD;
        if ( obj->type == G3DPLANETYPE      ) wname = EDITPLANE;
        if ( obj->type == G3DTORUSTYPE      ) wname = EDITTORUS;
        if ( obj->type == G3DBONETYPE       ) wname = EDITBONE;
        if ( obj->type == G3DUVMAPTYPE      ) wname = EDITUVMAP;
        if ( obj->type == G3DSUBDIVIDERTYPE ) wname = EDITSUBDIVIDER;
        if ( obj->type == G3DWIREFRAMETYPE  ) wname = EDITWIREFRAME;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( strcmp ( child_name, EDITOBJECT ) == 0x00 ) {
                updateObjectEdit ( child, gui );

                gtk_widget_show ( child );
            } else {
                if ( wname && ( strcmp ( child_name, wname ) == 0x00 ) ) {
                    if ( obj->type == G3DSPHERETYPE     ) updateSphereEdit     ( child, gui );
                    if ( obj->type == G3DCUBETYPE       ) updateCubeEdit       ( child, gui );
                    if ( obj->type == G3DCYLINDERTYPE   ) updateCylinderEdit   ( child, gui );
                    if ( obj->type == G3DPLANETYPE      ) updatePlaneEdit      ( child, gui );
                    if ( obj->type == G3DTORUSTYPE      ) updateTorusEdit      ( child, gui );
                    if ( obj->type == G3DMESHTYPE       ) updateMeshEdit       ( child, gui );
                    if ( obj->type == G3DBONETYPE       ) updateBoneEdit       ( child, gui );
                    if ( obj->type == G3DFFDTYPE        ) updateFFDEdit        ( child, gui );
                    if ( obj->type == G3DSYMMETRYTYPE   ) updateSymmetryEdit   ( child, gui );
                    if ( obj->type == G3DLIGHTTYPE      ) updateLightEdit      ( child, gui );
                    if ( obj->type == G3DUVMAPTYPE      ) updateUVMapEdit      ( child, gui );
                    if ( obj->type == G3DSUBDIVIDERTYPE ) updateSubdividerEdit ( child, gui );
                    if ( obj->type == G3DWIREFRAMETYPE  ) updateWireframeEdit  ( child, gui );

                    gtk_widget_show ( child );
                } else {
                    gtk_widget_hide ( child );
                }
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
/*** under ubuntu12, the Realize callback want called, I had to use Map event**/
static void Map ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateCurrentEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget *createCurrentEdit ( GtkWidget *parent, G3DUI *gui,
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

    g_signal_connect ( G_OBJECT (frm), "map", G_CALLBACK (Map), gui );

    /*** This is for all objects ***/
    createObjectEdit   ( frm, gui, EDITOBJECT  , 0,  0, 296,  32 );

    /*** This is type dependent: hidden if not of ***/
    /*** selected object type showed otherwise.   ***/
    createSphereEdit     ( frm, gui, EDITSPHERE    , 0, 32, 296, 320 );
    createCubeEdit       ( frm, gui, EDITCUBE      , 0, 32, 296, 320 );
    createCylinderEdit   ( frm, gui, EDITCYLINDER  , 0, 32, 296, 320 );
    createPlaneEdit      ( frm, gui, EDITPLANE     , 0, 32, 296, 320 );
    createTorusEdit      ( frm, gui, EDITTORUS     , 0, 32, 296, 320 );
    createMeshEdit       ( frm, gui, EDITMESH      , 0, 32, 320, 320 );
    createBoneEdit       ( frm, gui, EDITBONE      , 0, 32, 296, 320 );
    createFFDEdit        ( frm, gui, EDITFFD       , 0, 32, 296, 320 );
    createSymmetryEdit   ( frm, gui, EDITSYMMETRY  , 0, 32, 296, 320 );
    createSubdividerEdit ( frm, gui, EDITSUBDIVIDER, 0, 32, 296, 320 );
    createLightEdit      ( frm, gui, EDITLIGHT     , 0, 32, 296,  96 );
    createUVMapEdit      ( frm, gui, EDITUVMAP     , 0, 32, 296,  96 );
    createWireframeEdit  ( frm, gui, EDITWIREFRAME , 0, 32, 296,  96 );

    list_insert ( &gui->lcuredit, frm );

    gtk_widget_show ( frm );


    gtk_widget_show ( scr );


    return scr;
}

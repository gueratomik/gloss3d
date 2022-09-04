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
static GTK3G3DUIOBJECTEDIT *gtk3_g3duiobjectedit_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memsize = sizeof ( GTK3G3DUIOBJECTEDIT );
    GTK3G3DUIOBJECTEDIT *gtk3objedit = calloc ( 0x01, memsize );

    if ( gtk3objedit == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3objedit->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3objedit; 
}

/******************************************************************************/
void gtk3_g3duiobjectedit_update ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUILIGHTEDIT    *ligedit = ( GTK3G3DUILIGHTEDIT    * ) gtk3objedit->core.ligedit;
    GTK3G3DUICUBEEDIT     *cubedit = ( GTK3G3DUICUBEEDIT     * ) gtk3objedit->core.cubedit;
    GTK3G3DUICYLINDEREDIT *cyledit = ( GTK3G3DUICYLINDEREDIT * ) gtk3objedit->core.cyledit;
    GTK3G3DUIFFDEDIT      *ffdedit = ( GTK3G3DUIFFDEDIT      * ) gtk3objedit->core.ffdedit;
    GTK3G3DUIINSTANCEEDIT *insedit = ( GTK3G3DUIINSTANCEEDIT * ) gtk3objedit->core.insedit;

    G3DUI *gui = gtk3objedit->core.gui;
    G3DSCENE *sce = gui->sce;

    gtk_widget_hide ( ligedit->notebook );
    gtk_widget_hide ( cubedit->notebook );
    gtk_widget_hide ( cyledit->notebook );
    gtk_widget_hide ( ffdedit->notebook );
    gtk_widget_hide ( insedit->notebook );

    if ( sce ) {
        G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

        if ( obj ) {
            uint32_t same = g3dobjectlist_checkType ( sce->lsel, obj->type );

            if ( same ) {
                if ( obj->type == G3DLIGHTTYPE ) {
                    gtk3_g3duilightedit_update ( ligedit );

                    gtk_widget_show ( ligedit->notebook );
                }

                if ( obj->type == G3DCUBETYPE ) {
                    gtk3_g3duicubeedit_update ( cubedit );

                    gtk_widget_show ( cubedit->notebook );
                }

                if ( obj->type == G3DCYLINDERTYPE ) {
                    gtk3_g3duicylinderedit_update ( cyledit );

                    gtk_widget_show ( cyledit->notebook );
                }

                if ( obj->type == G3DFFDTYPE ) {
                    gtk3_g3duiffdedit_update ( ffdedit );

                    gtk_widget_show ( ffdedit->notebook );
                }

                if ( obj->type == G3DINSTANCETYPE ) {
                    gtk3_g3duiinstanceedit_update ( insedit );

                    gtk_widget_show ( insedit->notebook );
                }

            #ifdef unused
                if ( obj ) {
                    char *wname = NULL;

                    if ( obj->type == G3DSPHERETYPE          ) wname = EDITSPHERE;
                    if ( obj->type == G3DCUBETYPE            ) wname = EDITCUBE;
                    if ( obj->type == G3DCYLINDERTYPE        ) wname = EDITCYLINDER;
                    if ( obj->type == G3DMESHTYPE            ) wname = EDITMESH;
                    if ( obj->type == G3DLIGHTTYPE           ) wname = EDITLIGHT;
                    if ( obj->type == G3DSYMMETRYTYPE        ) wname = EDITSYMMETRY;
                    if ( obj->type == G3DFFDTYPE             ) wname = EDITFFD;
                    if ( obj->type == G3DPLANETYPE           ) wname = EDITPLANE;
                    if ( obj->type == G3DTORUSTYPE           ) wname = EDITTORUS;
                    if ( obj->type == G3DTUBETYPE            ) wname = EDITTUBE;
                    if ( obj->type == G3DBONETYPE            ) wname = EDITBONE;
                    if ( obj->type == G3DUVMAPTYPE           ) wname = EDITUVMAP;
                    if ( obj->type == G3DSUBDIVIDERTYPE      ) wname = EDITSUBDIVIDER;
                    if ( obj->type == G3DSPLINEREVOLVERTYPE  ) wname = EDITSPLINEREVOLVER;
                    if ( obj->type == G3DWIREFRAMETYPE       ) wname = EDITWIREFRAME;
                    if ( obj->type == G3DTEXTTYPE            ) wname = EDITTEXT;
                    if ( obj->type == G3DCAMERATYPE          ) wname = EDITCAMERA;
                    if ( obj->type == G3DMORPHERTYPE         ) wname = EDITMORPHER;
                    if ( obj->type == G3DINSTANCETYPE        ) wname = EDITINSTANCE;
                    if ( obj->type == G3DPARTICLEEMITTERTYPE ) wname = EDITPARTICLEEMITTER;

                    while ( children ) {
                        GtkWidget *child = ( GtkWidget * ) children->data;
                        const char *child_name = gtk_widget_get_name ( child );

                        if ( strcmp ( child_name, EDITOBJECT ) == 0x00 ) {
                            updateObjectEdit ( child, gui );

                            gtk_widget_show ( child );
                        } else {
                            if ( wname && ( strcmp ( child_name, wname ) == 0x00 ) ) {
                                if ( obj->type == G3DSPHERETYPE          ) updateSphereEdit          ( child, gui );
                                if ( obj->type == G3DCUBETYPE            ) updateCubeEdit            ( child, gui );
                                if ( obj->type == G3DCYLINDERTYPE        ) updateCylinderEdit        ( child, gui );
                                if ( obj->type == G3DPLANETYPE           ) updatePlaneEdit           ( child, gui );
                                if ( obj->type == G3DTORUSTYPE           ) updateTorusEdit           ( child, gui );
                                if ( obj->type == G3DTUBETYPE            ) updateTubeEdit            ( child, gui );
                                if ( obj->type == G3DMESHTYPE            ) updateMeshEdit            ( child, gui );
                                if ( obj->type == G3DBONETYPE            ) updateBoneEdit            ( child, gui );
                                if ( obj->type == G3DFFDTYPE             ) updateFFDEdit             ( child, gui );
                                if ( obj->type == G3DSYMMETRYTYPE        ) updateSymmetryEdit        ( child, gui );
                                if ( obj->type == G3DLIGHTTYPE           ) updateLightEdit           ( child, obj );
                                if ( obj->type == G3DUVMAPTYPE           ) updateUVMapEdit           ( child, gui );
                                if ( obj->type == G3DSUBDIVIDERTYPE      ) updateSubdividerEdit      ( child, obj );
                                if ( obj->type == G3DSPLINEREVOLVERTYPE  ) updateSplineRevolverEdit  ( child, gui );
                                if ( obj->type == G3DWIREFRAMETYPE       ) updateWireframeEdit       ( child, gui );
                                if ( obj->type == G3DTEXTTYPE            ) updateTextEdit            ( child, gui );
                                if ( obj->type == G3DCAMERATYPE          ) updateCameraEdit          ( child, gui );
                                if ( obj->type == G3DMORPHERTYPE         ) updateMorpherEdit         ( child, gui );
                                if ( obj->type == G3DINSTANCETYPE        ) updateInstanceEdit        ( child, obj );
                                if ( obj->type == G3DPARTICLEEMITTERTYPE ) updateParticleEmitterEdit ( child, obj );

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
            #endif
            }
        }
    }
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIOBJECTEDIT *gtk3objedit = ( GTK3G3DUIOBJECTEDIT * ) user_data;

    free ( gtk3objedit );
}

/******************************************************************************/
/* under ubuntu12, the Realize callback wasnt called, I had to use Map event  */
static void Map ( GtkWidget *widget,
                  gpointer   user_data ) {
    GTK3G3DUIOBJECTEDIT *gtk3objedit = ( GTK3G3DUIOBJECTEDIT * ) user_data;

    gtk3_g3duiobjectedit_update ( gtk3objedit );
}

/******************************************************************************/
static void createLightEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUILIGHTEDIT *gtk3ligedit;

    gtk3ligedit = gtk3_g3duilightedit_create ( gtk3objedit->fixed,
                                               gtk3gui,
                                               "Edit Light" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->fixed), gtk3ligedit->notebook, 0, 0 );

    gtk3objedit->core.ligedit = ( G3DUILIGHTEDIT * ) gtk3ligedit;
}

/******************************************************************************/
static void createCubeEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUICUBEEDIT *gtk3cubedit;

    gtk3cubedit = gtk3_g3duicubeedit_create ( gtk3objedit->fixed,
                                              gtk3gui,
                                              "Edit cube" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->fixed), gtk3cubedit->notebook, 0, 0 );

    gtk3objedit->core.cubedit = ( G3DUICUBEEDIT * ) gtk3cubedit;
}

/******************************************************************************/
static void createCylinderEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUICYLINDEREDIT *gtk3cyledit;

    gtk3cyledit = gtk3_g3duicylinderedit_create ( gtk3objedit->fixed,
                                                  gtk3gui,
                                                  "Edit cylinder" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->fixed), gtk3cyledit->notebook, 0, 0 );

    gtk3objedit->core.cyledit = ( G3DUICYLINDEREDIT * ) gtk3cyledit;
}

/******************************************************************************/
static void createFFDEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUIFFDEDIT *gtk3ffdedit;

    gtk3ffdedit = gtk3_g3duiffdedit_create ( gtk3objedit->fixed,
                                             gtk3gui,
                                             "Edit FFD" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->fixed), gtk3ffdedit->notebook, 0, 0 );

    gtk3objedit->core.ffdedit = ( GTK3G3DUIFFDEDIT * ) gtk3ffdedit;
}

/******************************************************************************/
static void createInstanceEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUIINSTANCEEDIT *gtk3insedit;

    gtk3insedit = gtk3_g3duiinstanceedit_create ( gtk3objedit->fixed,
                                                  gtk3gui,
                                                  "Edit Instance" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->fixed), gtk3insedit->notebook, 0, 0 );

    gtk3objedit->core.insedit = ( GTK3G3DUIINSTANCEEDIT * ) gtk3insedit;
}

/******************************************************************************/
GTK3G3DUIOBJECTEDIT *gtk3_g3duiobjectedit_create ( GtkWidget *parent, 
                                                   GTK3G3DUI *gtk3gui,
                                                   char      *name ) {
    GTK3G3DUIOBJECTEDIT *gtk3objedit = gtk3_g3duiobjectedit_new ( gtk3gui );
    GtkWidget *scrolled = gtk_scrolled_window_new ( NULL, NULL );
    GtkWidget *fixed = gtk_fixed_new ( );

    gtk3objedit->scrolled = scrolled;
    gtk3objedit->fixed    = fixed;

    gtk_widget_set_name ( scrolled, name );
    gtk_widget_set_name ( fixed   , name );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_container_add ( GTK_CONTAINER(scrolled), fixed );
#else
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(scrolled), fixed );
#endif

    g_signal_connect ( G_OBJECT (fixed), "map"    , G_CALLBACK (Map)    , gtk3objedit );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3objedit );

    /*** This is for all objects ***/
/*    createObjectEdit   ( fixed, gui, EDITOBJECT  , 0,  0, 296,  32 );*/

    /*** This is type dependent: hidden if not of ***/
    /*** selected object type showed otherwise.   ***/
/*
    createSphereEdit          ( frm, gui, EDITSPHERE         , 0, 32, 296, 320 );
    createCubeEdit            ( frm, gui, EDITCUBE           , 0, 32, 296, 320 );
    createCylinderEdit        ( frm, gui, EDITCYLINDER       , 0, 32, 296, 320 );
    createPlaneEdit           ( frm, gui, EDITPLANE          , 0, 32, 296, 320 );
    createTorusEdit           ( frm, gui, EDITTORUS          , 0, 32, 296, 320 );
    createTubeEdit            ( frm, gui, EDITTUBE           , 0, 32, 296, 320 );
    createMeshEdit            ( frm, gui, EDITMESH           , 0, 32, 320, 320 );
    createBoneEdit            ( frm, gui, EDITBONE           , 0, 32, 296, 320 );
    createFFDEdit             ( frm, gui, EDITFFD            , 0, 32, 296, 320 );
    createSymmetryEdit        ( frm, gui, EDITSYMMETRY       , 0, 32, 296, 320 );
    createSubdividerEdit      ( frm, gui, EDITSUBDIVIDER     , 0, 32, 296, 320 );
    createSplineRevolverEdit  ( frm, gui, EDITSPLINEREVOLVER , 0, 32, 296, 320 );
*/
    createLightEdit    ( gtk3objedit );
    createCubeEdit     ( gtk3objedit );
    createCylinderEdit ( gtk3objedit );
    createFFDEdit      ( gtk3objedit );
    createInstanceEdit ( gtk3objedit );
/*
    createUVMapEdit           ( frm, gui, EDITUVMAP          , 0, 32, 296,  96 );
    createWireframeEdit       ( frm, gui, EDITWIREFRAME      , 0, 32, 296,  96 );
    createTextEdit            ( frm, gui, EDITTEXT           , 0, 32, 296,  96 );
    createCameraEdit          ( frm, gui, EDITCAMERA         , 0, 32, 296,  96 );
    createMorpherEdit         ( frm, gui, EDITMORPHER        , 0, 32, 296,  96 );
    createInstanceEdit        ( frm, gui, EDITINSTANCE       , 0, 32, 296,  96 );
    createParticleEmitterEdit ( frm, gui, EDITPARTICLEEMITTER, 0, 32, 296,  96 );
*/

    gtk_widget_show ( fixed    );
    gtk_widget_show ( scrolled );


    return gtk3objedit;
}

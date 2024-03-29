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
    GTK3G3DUILIGHTEDIT           *ligedit = ( GTK3G3DUILIGHTEDIT           * ) gtk3objedit->core.ligedit;
    GTK3G3DUICUBEEDIT            *cubedit = ( GTK3G3DUICUBEEDIT            * ) gtk3objedit->core.cubedit;
    GTK3G3DUICYLINDEREDIT        *cyledit = ( GTK3G3DUICYLINDEREDIT        * ) gtk3objedit->core.cyledit;
    GTK3G3DUIFFDEDIT             *ffdedit = ( GTK3G3DUIFFDEDIT             * ) gtk3objedit->core.ffdedit;
    GTK3G3DUIINSTANCEEDIT        *insedit = ( GTK3G3DUIINSTANCEEDIT        * ) gtk3objedit->core.insedit;
    GTK3G3DUIPARTICLEEMITTEREDIT *pemedit = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) gtk3objedit->core.pemedit;
    GTK3G3DUITUBEEDIT            *tubedit = ( GTK3G3DUITUBEEDIT            * ) gtk3objedit->core.tubedit;
    GTK3G3DUITORUSEDIT           *toredit = ( GTK3G3DUITORUSEDIT           * ) gtk3objedit->core.toredit;
    GTK3G3DUISPHEREEDIT          *sphedit = ( GTK3G3DUISPHEREEDIT          * ) gtk3objedit->core.sphedit;
    GTK3G3DUIPLANEEDIT           *plnedit = ( GTK3G3DUIPLANEEDIT           * ) gtk3objedit->core.plnedit;
    GTK3G3DUIWIREFRAMEEDIT       *wiredit = ( GTK3G3DUIWIREFRAMEEDIT       * ) gtk3objedit->core.wiredit;
    GTK3G3DUISUBDIVIDEREDIT      *sdredit = ( GTK3G3DUISUBDIVIDEREDIT      * ) gtk3objedit->core.sdredit;
    GTK3G3DUISYMMETRYEDIT        *symedit = ( GTK3G3DUISYMMETRYEDIT        * ) gtk3objedit->core.symedit;
    GTK3G3DUISPLINEREVOLVEREDIT  *srvedit = ( GTK3G3DUISPLINEREVOLVEREDIT  * ) gtk3objedit->core.srvedit;
    GTK3G3DUIMESHEDIT            *mesedit = ( GTK3G3DUIMESHEDIT            * ) gtk3objedit->core.mesedit;
    GTK3G3DUIBONEEDIT            *bonedit = ( GTK3G3DUIBONEEDIT            * ) gtk3objedit->core.bonedit;
    GTK3G3DUICAMERAEDIT          *camedit = ( GTK3G3DUICAMERAEDIT          * ) gtk3objedit->core.camedit;
    GTK3G3DUITEXTEDIT            *txtedit = ( GTK3G3DUITEXTEDIT            * ) gtk3objedit->core.txtedit;
    GTK3G3DUIMORPHEREDIT         *mpredit = ( GTK3G3DUIMORPHEREDIT         * ) gtk3objedit->core.mpredit;

    G3DUI *gui = gtk3objedit->core.gui;

    G3DSCENE *sce = gui->sce;

    if ( ligedit ) gtk_widget_hide ( ligedit->notebook );
    if ( cubedit ) gtk_widget_hide ( cubedit->notebook );
    if ( cyledit ) gtk_widget_hide ( cyledit->notebook );
    if ( ffdedit ) gtk_widget_hide ( ffdedit->notebook );
    if ( insedit ) gtk_widget_hide ( insedit->notebook );
    if ( pemedit ) gtk_widget_hide ( pemedit->notebook );
    if ( tubedit ) gtk_widget_hide ( tubedit->notebook );
    if ( toredit ) gtk_widget_hide ( toredit->notebook );
    if ( sphedit ) gtk_widget_hide ( sphedit->notebook );
    if ( plnedit ) gtk_widget_hide ( plnedit->notebook );
    if ( wiredit ) gtk_widget_hide ( wiredit->notebook );
    if ( sdredit ) gtk_widget_hide ( sdredit->notebook );
    if ( symedit ) gtk_widget_hide ( symedit->notebook );
    if ( srvedit ) gtk_widget_hide ( srvedit->notebook );
    if ( mesedit ) gtk_widget_hide ( mesedit->notebook );
    if ( bonedit ) gtk_widget_hide ( bonedit->notebook );
    if ( camedit ) gtk_widget_hide ( camedit->notebook );
    if ( txtedit ) gtk_widget_hide ( txtedit->notebook );
    if ( mpredit ) gtk_widget_hide ( mpredit->notebook );

    gtk_widget_set_sensitive ( gtk3objedit->nameEntry, FALSE );


    if ( sce ) {
        G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

        if ( obj ) {
            uint32_t same = g3dobjectlist_checkType ( sce->lsel, obj->type );

            if ( gtk3objedit->nameEntry ) {
                gtk_widget_set_sensitive ( gtk3objedit->nameEntry, TRUE );

                gtk_entry_set_text ( gtk3objedit->nameEntry, obj->name );
            }

            if ( same ) {
                if ( obj->type == G3DLIGHTTYPE ) {
                    gtk3_g3duilightedit_update ( ligedit, NULL );

                    gtk_widget_show ( ligedit->notebook );
                }

                if ( obj->type == G3DCUBETYPE ) {
                    gtk3_g3duicubeedit_update ( cubedit );

                    gtk_widget_show ( cubedit->notebook );
                }

                if ( obj->type == G3DTUBETYPE ) {
                    gtk3_g3duitubeedit_update ( tubedit );

                    gtk_widget_show ( tubedit->notebook );
                }

                if ( obj->type == G3DTEXTTYPE ) {
                    gtk3_g3duitextedit_update ( txtedit );

                    gtk_widget_show ( txtedit->notebook );
                }

                if ( obj->type == G3DTORUSTYPE ) {
                    gtk3_g3duitorusedit_update ( toredit );

                    gtk_widget_show ( toredit->notebook );
                }

                if ( obj->type == G3DCAMERATYPE ) {
                    gtk3_g3duicameraedit_update ( camedit, NULL );

                    gtk_widget_show ( camedit->notebook );
                }

                if ( obj->type == G3DMESHTYPE ) {
                    gtk3_g3duimeshedit_update ( mesedit );

                    gtk_widget_show ( mesedit->notebook );
                }

                if ( obj->type == G3DMORPHERTYPE ) {
                    gtk3_g3duimorpheredit_update ( mpredit );

                    gtk_widget_show ( mpredit->notebook );
                }

                if ( obj->type == G3DSPHERETYPE ) {
                    gtk3_g3duisphereedit_update ( sphedit );

                    gtk_widget_show ( sphedit->notebook );
                }

                if ( obj->type == G3DBONETYPE ) {
                    gtk3_g3duiboneedit_update ( bonedit );

                    gtk_widget_show ( bonedit->notebook );
                }

                if ( obj->type == G3DSUBDIVIDERTYPE ) {
                    gtk3_g3duisubdivideredit_update ( sdredit );

                    gtk_widget_show ( sdredit->notebook );
                }

                if ( obj->type == G3DPLANETYPE ) {
                    gtk3_g3duiplaneedit_update ( plnedit );

                    gtk_widget_show ( plnedit->notebook );
                }

                if ( obj->type == G3DWIREFRAMETYPE ) {
                    gtk3_g3duiwireframeedit_update ( wiredit );

                    gtk_widget_show ( wiredit->notebook );
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


                if ( obj->type == G3DSYMMETRYTYPE ) {
                    gtk3_g3duisymmetryedit_update ( symedit );

                    gtk_widget_show ( symedit->notebook );
                }

                if ( obj->type == G3DSPLINEREVOLVERTYPE ) {
                    gtk3_g3duisplinerevolveredit_update ( srvedit );

                    gtk_widget_show ( srvedit->notebook );
                }

                if ( obj->type == G3DPARTICLEEMITTERTYPE ) {
                    gtk3_g3duiparticleemitteredit_update ( pemedit, NULL );

                    gtk_widget_show ( pemedit->notebook );
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
static void createLightEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUILIGHTEDIT *gtk3ligedit;

    gtk3ligedit = gtk3_g3duilightedit_create ( gtk3objedit->objectFixed,
                                               gtk3gui,
                                               "Edit Light",
                                               0x00 );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3ligedit->notebook, 0, 0 );

    gtk3objedit->core.ligedit = ( G3DUILIGHTEDIT * ) gtk3ligedit;
}

/******************************************************************************/
static void createCubeEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUICUBEEDIT *gtk3cubedit;

    gtk3cubedit = gtk3_g3duicubeedit_create ( gtk3objedit->objectFixed,
                                              gtk3gui,
                                              "Edit cube" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3cubedit->notebook, 0, 0 );

    gtk3objedit->core.cubedit = ( G3DUICUBEEDIT * ) gtk3cubedit;
}

/******************************************************************************/
static void createTubeEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUITUBEEDIT *gtk3tubedit;

    gtk3tubedit = gtk3_g3duitubeedit_create ( gtk3objedit->objectFixed,
                                              gtk3gui,
                                              "Edit tube" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3tubedit->notebook, 0, 0 );

    gtk3objedit->core.tubedit = ( G3DUITUBEEDIT * ) gtk3tubedit;
}

/******************************************************************************/
static void createTorusEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUITORUSEDIT *gtk3toredit;

    gtk3toredit = gtk3_g3duitorusedit_create ( gtk3objedit->objectFixed,
                                               gtk3gui,
                                               "Edit torus" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3toredit->notebook, 0, 0 );

    gtk3objedit->core.toredit = ( G3DUITORUSEDIT * ) gtk3toredit;
}

/******************************************************************************/
static void createSphereEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUISPHEREEDIT *gtk3sphedit;

    gtk3sphedit = gtk3_g3duisphereedit_create ( gtk3objedit->objectFixed,
                                                gtk3gui,
                                                "Edit sphere" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3sphedit->notebook, 0, 0 );

    gtk3objedit->core.sphedit = ( GTK3G3DUISPHEREEDIT * ) gtk3sphedit;
}

/******************************************************************************/
static void createPlaneEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUIPLANEEDIT *gtk3plnedit;

    gtk3plnedit = gtk3_g3duiplaneedit_create ( gtk3objedit->objectFixed,
                                               gtk3gui,
                                               "Edit plane" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3plnedit->notebook, 0, 0 );

    gtk3objedit->core.plnedit = ( GTK3G3DUIPLANEEDIT * ) gtk3plnedit;
}

/******************************************************************************/
static void createCylinderEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUICYLINDEREDIT *gtk3cyledit;

    gtk3cyledit = gtk3_g3duicylinderedit_create ( gtk3objedit->objectFixed,
                                                  gtk3gui,
                                                  "Edit cylinder" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3cyledit->notebook, 0, 0 );

    gtk3objedit->core.cyledit = ( G3DUICYLINDEREDIT * ) gtk3cyledit;
}

/******************************************************************************/
static void createFFDEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUIFFDEDIT *gtk3ffdedit;

    gtk3ffdedit = gtk3_g3duiffdedit_create ( gtk3objedit->objectFixed,
                                             gtk3gui,
                                             "Edit FFD" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3ffdedit->notebook, 0, 0 );

    gtk3objedit->core.ffdedit = ( GTK3G3DUIFFDEDIT * ) gtk3ffdedit;
}

/******************************************************************************/
static void createWireframeEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUIWIREFRAMEEDIT *gtk3wiredit;

    gtk3wiredit = gtk3_g3duiwireframeedit_create ( gtk3objedit->objectFixed,
                                                   gtk3gui,
                                                   "Edit Wireframe" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3wiredit->notebook, 0, 0 );

    gtk3objedit->core.wiredit = ( GTK3G3DUIWIREFRAMEEDIT * ) gtk3wiredit;
}

/******************************************************************************/
static void createInstanceEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUIINSTANCEEDIT *gtk3insedit;

    gtk3insedit = gtk3_g3duiinstanceedit_create ( gtk3objedit->objectFixed,
                                                  gtk3gui,
                                                  "Edit Instance" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3insedit->notebook, 0, 0 );

    gtk3objedit->core.insedit = ( GTK3G3DUIINSTANCEEDIT * ) gtk3insedit;
}

/******************************************************************************/
static void createSubdividerEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUISUBDIVIDEREDIT *gtk3subedit;

    gtk3subedit = gtk3_g3duisubdivideredit_create ( gtk3objedit->objectFixed,
                                                    gtk3gui,
                                                    "Edit Subdivider" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3subedit->notebook, 0, 0 );

    gtk3objedit->core.sdredit = ( GTK3G3DUISUBDIVIDEREDIT * ) gtk3subedit;
}

/******************************************************************************/
static void createBoneEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUIBONEEDIT *gtk3bonedit;

    gtk3bonedit = gtk3_g3duiboneedit_create ( gtk3objedit->objectFixed,
                                              gtk3gui,
                                              "Edit Bone" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3bonedit->notebook, 0, 0 );

    gtk3objedit->core.bonedit = ( GTK3G3DUIBONEEDIT * ) gtk3bonedit;
}

/******************************************************************************/
static void createMeshEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUIMESHEDIT *gtk3mesedit;

    gtk3mesedit = gtk3_g3duimeshedit_create ( gtk3objedit->objectFixed,
                                              gtk3gui,
                                              "Edit Mesh" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3mesedit->notebook, 0, 0 );

    gtk3objedit->core.mesedit = ( GTK3G3DUIMESHEDIT * ) gtk3mesedit;
}

/******************************************************************************/
static void createSymmetryEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUISYMMETRYEDIT *gtk3symedit;

    gtk3symedit = gtk3_g3duisymmetryedit_create ( gtk3objedit->objectFixed,
                                                  gtk3gui,
                                                  "Edit Symmetry" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3symedit->notebook, 0, 0 );

    gtk3objedit->core.symedit = ( GTK3G3DUISYMMETRYEDIT * ) gtk3symedit;
}

/******************************************************************************/
static void createSplineRevolverEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUISPLINEREVOLVEREDIT *gtk3srvedit;

    gtk3srvedit = gtk3_g3duisplinerevolveredit_create ( gtk3objedit->objectFixed,
                                                        gtk3gui,
                                                        "Edit Spline Revolver" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3srvedit->notebook, 0, 0 );

    gtk3objedit->core.srvedit = ( GTK3G3DUISPLINEREVOLVEREDIT * ) gtk3srvedit;
}

/******************************************************************************/
static void createParticleEmitterEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUIPARTICLEEMITTEREDIT *gtk3pemedit;

    gtk3pemedit = gtk3_g3duiparticleemitteredit_create ( gtk3objedit->objectFixed,
                                                         gtk3gui,
                                                         "Edit Particle Emitter",
                                                         0x00 );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3pemedit->notebook, 0, 0 );

    gtk3objedit->core.pemedit = ( GTK3G3DUIPARTICLEEMITTEREDIT * ) gtk3pemedit;
}

/******************************************************************************/
static void createCameraEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUICAMERAEDIT *gtk3camedit;

    gtk3camedit = gtk3_g3duicameraedit_create ( gtk3objedit->objectFixed,
                                                gtk3gui,
                                                "Edit Camera",
                                                0x00 );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3camedit->notebook, 0, 0 );

    gtk3objedit->core.camedit = ( GTK3G3DUICAMERAEDIT * ) gtk3camedit;
}

/******************************************************************************/
static void createTextEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUITEXTEDIT *gtk3txtedit;

    gtk3txtedit = gtk3_g3duitextedit_create ( gtk3objedit->objectFixed,
                                              gtk3gui,
                                              "Edit Text" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3txtedit->notebook, 0, 0 );

    gtk3objedit->core.txtedit = ( GTK3G3DUITEXTEDIT * ) gtk3txtedit;
}

/******************************************************************************/
static void createMorpherEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    GTK3G3DUIMORPHEREDIT *gtk3mpredit;

    gtk3mpredit = gtk3_g3duimorpheredit_create ( gtk3objedit->objectFixed,
                                                 gtk3gui,
                                                 "Edit Morpher" );

    gtk_fixed_put ( GTK_FIXED(gtk3objedit->objectFixed), gtk3mpredit->notebook, 0, 0 );

    gtk3objedit->core.mpredit = ( GTK3G3DUIMORPHEREDIT * ) gtk3mpredit;
}

/******************************************************************************/
static void nameObjectCbk ( GtkWidget *widget, 
                            GdkEvent  *event,
                            gpointer   user_data ) {
    GTK3G3DUIOBJECTEDIT *gtk3objedit = ( GTK3G3DUIOBJECTEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;
    const char *object_name = gtk_entry_get_text ( GTK_ENTRY(widget) );
    uint64_t ret;

    if ( gtk3objedit->core.gui->lock ) return;

    ret = g3duiobjectedit_name ( &gtk3objedit->core, object_name );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
static void createObjectEdit ( GTK3G3DUIOBJECTEDIT *gtk3objedit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3objedit->core.gui;

    gtk3objedit->scrolled    = ui_gtk_scrolled_window_new ( CLASS_MAIN, NULL, NULL );
    gtk3objedit->objectFixed = ui_gtk_fixed_new ( CLASS_MAIN );
    gtk3objedit->nameEntry   = ui_createCharText ( gtk3objedit->fixed, gtk3objedit, EDITOBJECTNAME, CLASS_MAIN, 0,  0, 96, 96, 20, nameObjectCbk  );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_container_add ( GTK_CONTAINER(gtk3objedit->scrolled), gtk3objedit->objectFixed );
#else
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(gtk3objedit->scrolled), gtk3objedit->objectFixed );
#endif

    gtk_fixed_put ( gtk3objedit->fixed, gtk3objedit->scrolled , 0, 20 );

    createLightEdit           ( gtk3objedit );
    createCubeEdit            ( gtk3objedit );
    createTubeEdit            ( gtk3objedit );
    createPlaneEdit           ( gtk3objedit );
    createSphereEdit          ( gtk3objedit );
    createTorusEdit           ( gtk3objedit );
    createCylinderEdit        ( gtk3objedit );
    createFFDEdit             ( gtk3objedit );
    createInstanceEdit        ( gtk3objedit );
    createParticleEmitterEdit ( gtk3objedit );
    createWireframeEdit       ( gtk3objedit );
    createSubdividerEdit      ( gtk3objedit );
    createSymmetryEdit        ( gtk3objedit );
    createSplineRevolverEdit  ( gtk3objedit );
    createMeshEdit            ( gtk3objedit );
    createBoneEdit            ( gtk3objedit );
    createCameraEdit          ( gtk3objedit );
    createTextEdit            ( gtk3objedit );
    createMorpherEdit         ( gtk3objedit );
}

/******************************************************************************/
void gtk3_g3duiobjectedit_resize ( GTK3G3DUIOBJECTEDIT *gtk3objedit,
                                   uint32_t             width,
                                   uint32_t             height ) {
    if ( gtk3objedit->scrolled ) {
        gtk_widget_set_size_request ( gtk3objedit->scrolled,
                                      width,
                                      height - 40 );
    }
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIOBJECTEDIT *gtk3objedit = ( GTK3G3DUIOBJECTEDIT * ) user_data;

    free ( gtk3objedit );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIOBJECTEDIT *gtk3objedit = ( GTK3G3DUIOBJECTEDIT * ) user_data;

    gtk3_g3duiobjectedit_update ( gtk3objedit );
}

/******************************************************************************/
GTK3G3DUIOBJECTEDIT *gtk3_g3duiobjectedit_create ( GtkWidget *parent, 
                                                   GTK3G3DUI *gtk3gui,
                                                   char      *name ) {
    GTK3G3DUIOBJECTEDIT *gtk3objedit = gtk3_g3duiobjectedit_new ( gtk3gui );

    gtk3objedit->fixed   = ui_gtk_fixed_new ( CLASS_MAIN );

    g_signal_connect ( G_OBJECT (gtk3objedit->fixed), "realize", G_CALLBACK (Realize), gtk3objedit );
    g_signal_connect ( G_OBJECT (gtk3objedit->fixed), "destroy", G_CALLBACK (Destroy), gtk3objedit );

    /*** This is type dependent: hidden if not of ***/
    /*** selected object type showed otherwise.   ***/

    createObjectEdit ( gtk3objedit );

/*
    createMorpherEdit         ( frm, gui, EDITMORPHER        , 0, 32, 296,  96 );
*/

    gtk_widget_show_all ( gtk3objedit->fixed );

    return gtk3objedit;
}

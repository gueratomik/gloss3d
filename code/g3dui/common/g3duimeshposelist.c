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
#include <g3dui.h>

/******************************************************************************/
G3DMESHPOSE *common_g3duimeshposelist_getMeshPose ( G3DUI *gui,
                                                    float  mouse_x,
                                                    float  mouse_y ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESHPOSEEXTENSION *ext = ( G3DMESHPOSEEXTENSION * ) g3dobject_getExtensionByID ( obj, MESHPOSEEXTENSION, 0x00 );

        if ( ext ) {
            LIST *ltmpmps = ext->lmps;
            float y = 0;

            while ( ltmpmps ) {
                G3DMESHPOSE *mps = ( G3DMESHPOSE * ) ltmpmps->data;
                float y1, y2;

                y1 = y;
                y2 = y1 + LISTINDENT;

                if ( ( mouse_y > y1 ) && ( mouse_y < y2 ) ) {
                    return mps;
                }

                y = y2;

                ltmpmps = ltmpmps->next;
            }
        }
    }

    return NULL;
}

/******************************************************************************/
void common_g3duimeshposelist_deleteCurrentPoseCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESHPOSEEXTENSION *ext = ( G3DMESHPOSEEXTENSION * ) g3dobject_getExtensionByID ( obj, MESHPOSEEXTENSION, 0x00 );

        if ( ext ) {
            if ( ext->curmps ) {
                g3dmeshposeextension_removePose ( ext, ext->curmps );

                g3dui_redrawAllMeshPoseList ( gui );
            }
        }
    }
}

/******************************************************************************/
void common_g3duimeshposelist_createPoseCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESHPOSEEXTENSION *ext = ( G3DMESHPOSEEXTENSION * ) g3dobject_getExtensionByID ( obj, MESHPOSEEXTENSION, 0x00 );

        if ( ext ) {
            char buf[0x20];

            snprintf ( buf, 0x20, "MeshPose%02i", ext->nbmps );

            g3dmeshposeextension_createPose ( ext, buf );

            g3dui_redrawAllMeshPoseList ( gui );
        }
    }
}

/******************************************************************************/
void common_g3duimeshposelist_renameCurrentPoseCbk ( G3DUI *gui, 
                                                     char  *mpsname ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESHPOSEEXTENSION *ext = ( G3DMESHPOSEEXTENSION * ) g3dobject_getExtensionByID ( obj, MESHPOSEEXTENSION, 0x00 );

        if ( ext ) {
            if ( ext->curmps ) {
                g3dmeshpose_name ( ext->curmps, ( char * ) mpsname );

                g3dui_redrawAllMeshPoseList ( gui );
            }
        }
    }
}

/******************************************************************************/
void common_g3duimeshposelist_deleteSelectedPoseCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESHPOSEEXTENSION *ext = ( G3DMESHPOSEEXTENSION * ) g3dobject_getExtensionByID ( obj, MESHPOSEEXTENSION, 0x00 );

        if ( ext ) {
            if ( ext->curmps ) {
                G3DMESHPOSE *curmps = ext->curmps;

                g3dmeshposeextension_removePose ( ext, curmps );

                /*** update vertex painting ***/
                /*g3dmesh_update ( mes, NULL,
                                      NULL,
                                      NULL,
                                      UPDATEVERTEXNORMAL |
                                      UPDATEFACENORMAL |
                                      RESETMODIFIERS, gui->engine_flags );*/

                g3dui_redrawGLViews ( gui );
                g3dui_updateAllCurrentEdit ( gui );
            }
        }
    }
}

/******************************************************************************/
void common_g3duimeshposelist_selectPoseCbk ( G3DUI *gui, G3DMESHPOSE *mps ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESHPOSEEXTENSION *ext = ( G3DMESHPOSEEXTENSION * ) g3dobject_getExtensionByID ( obj, MESHPOSEEXTENSION, 0x00 );
        G3DMESHPOSE *curmps = ext->curmps;
        G3DMESH *mes = ( G3DMESH * ) obj;

        /*** deselect previsouly selected meshpose ***/
        if ( ext->curmps ) {
            G3DMESHPOSE *curmps = ext->curmps;

            g3dmeshposeextension_unselectPose ( ext, curmps );

            /*** update vertex painting ***/
            /*g3dmesh_update ( mes, curgrp->lver,
                                  NULL,
                                  NULL,
                                  UPDATEMODIFIERS, gui->engine_flags );*/
        }

        g3dmeshposeextension_selectPose ( ext, mps );

        /*** update vertex painting ***/
        /*g3dmesh_update ( mes, grp->lver,
                              NULL,
                              NULL,
                              RESETMODIFIERS, gui->engine_flags );*/

        g3dui_redrawGLViews ( gui );
        g3dui_updateAllCurrentEdit ( gui );
    }
}

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


#ifdef unused
/******************************************************************************/
G3DMORPHERMESHPOSE *g3duimeshposelist_getMeshPose ( G3DUIMESHPOSELIST *gmp,
                                                    float              mouse_x,
                                                    float              mouse_y ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMORPHERMESHPOSEEXTENSION *ext = ( G3DMORPHERMESHPOSEEXTENSION * ) g3dobject_getExtensionByID ( obj, MESHPOSEEXTENSION, 0x00 );

        if ( ext ) {
            LIST *ltmpmps = ext->lmps;
            float y = 0;

            while ( ltmpmps ) {
                G3DMORPHERMESHPOSE *mps = ( G3DMORPHERMESHPOSE * ) ltmpmps->data;
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
uint64_t g3duimeshposelist_deleteCurrentPose ( G3DUIMESHPOSELIST *gmp ) {
    G3DUI *gui = gmp->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );


    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMORPHERMESHPOSEEXTENSION *ext = ( G3DMORPHERMESHPOSEEXTENSION * ) g3dobject_getExtensionByID ( obj, MESHPOSEEXTENSION, 0x00 );

        if ( ext ) {
            if ( ext->curmps ) {
                g3dmeshposeextension_removePose ( ext, ext->curmps );

                /*g3dui_redrawAllMeshPoseList ( gui );*/
            }
        }
    }

    return UPDATECURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3duimeshposelist_createPose ( G3DUIMESHPOSELIST *gmp ) {
    G3DUI *gui = gmp->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMORPHERMESHPOSEEXTENSION *ext = ( G3DMORPHERMESHPOSEEXTENSION * ) g3dobject_getExtensionByID ( obj, MESHPOSEEXTENSION, 0x00 );

        if ( ext ) {
            char buf[0x20];

            snprintf ( buf, 0x20, "MeshPose%02i", ext->nbmps );

            g3dmeshposeextension_createPose ( ext, buf );

            /*g3dui_redrawAllMeshPoseList ( gui );*/
        }
    }

    return UPDATECURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3duimeshposelist_renameCurrentPose ( G3DUIMESHPOSELIST *gmp, 
                                                  char              *mpsname ) {
    G3DUI *gui = gmp->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMORPHERMESHPOSEEXTENSION *ext = ( G3DMORPHERMESHPOSEEXTENSION * ) g3dobject_getExtensionByID ( obj, MESHPOSEEXTENSION, 0x00 );

        if ( ext ) {
            if ( ext->curmps ) {
                g3dmeshpose_name ( ext->curmps, ( char * ) mpsname );

                /*g3dui_redrawAllMeshPoseList ( gui );*/
            }
        }
    }

    return UPDATECURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3duimeshposelist_deleteSelectedPose ( G3DUIMESHPOSELIST *gmp ) {
    G3DUI *gui = gmp->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMORPHERMESHPOSEEXTENSION *ext = ( G3DMORPHERMESHPOSEEXTENSION * ) g3dobject_getExtensionByID ( obj, MESHPOSEEXTENSION, 0x00 );

        if ( ext ) {
            if ( ext->curmps ) {
                G3DMORPHERMESHPOSE *curmps = ext->curmps;

                g3dmeshposeextension_removePose ( ext, curmps );
            }
        }
    }

    return REDRAWVIEW | UPDATECURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3duimeshposelist_selectPose ( G3DUIMESHPOSELIST  *gmp,
                                           G3DMORPHERMESHPOSE *mps ) {
    G3DUI *gui = gmp->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMORPHERMESHPOSEEXTENSION *ext = ( G3DMORPHERMESHPOSEEXTENSION * ) g3dobject_getExtensionByID ( obj, MESHPOSEEXTENSION, 0x00 );
        G3DMORPHERMESHPOSE *curmps = ext->curmps;
        G3DMESH *mes = ( G3DMESH * ) obj;

        /*** deselect previsouly selected meshpose ***/
        if ( ext->curmps ) {
            G3DMORPHERMESHPOSE *curmps = ext->curmps;

            g3dmeshposeextension_unselectPose ( ext, curmps );
        }

        g3dmeshposeextension_selectPose ( ext, mps );
    }

    return REDRAWVIEW | UPDATECURRENTOBJECT;
}

#endif

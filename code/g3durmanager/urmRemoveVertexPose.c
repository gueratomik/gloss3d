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
#include <g3durmanager.h>

typedef struct _URMREMOVEVERTEXPOSE {
    G3DSCENE           *sce;
    G3DMORPHER         *mpr;
    G3DMORPHERMESHPOSE *mpose;
    LIST               *lver;
    G3DVECTOR          *pos;
} URMREMOVEVERTEXPOSE;

/******************************************************************************/
static URMREMOVEVERTEXPOSE *urmremovevertexpose_new ( G3DSCENE      *sce,
                                                      G3DMORPHER    *mpr,
                                                 G3DMORPHERMESHPOSE *mpose,
                                                      LIST          *lver ) {
    uint32_t ssize = sizeof ( URMREMOVEVERTEXPOSE );

    URMREMOVEVERTEXPOSE *rvp = ( URMREMOVEVERTEXPOSE * ) calloc ( 0x01, ssize );

    if ( rvp == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    rvp->sce   = sce;
    rvp->mpr   = mpr;
    rvp->mpose = mpose;
    rvp->lver  = lver;
    rvp->pos   = g3dmorpher_getMeshPoseArrayFromList ( mpr, mpose, lver );

    return rvp;
}

/******************************************************************************/
static void urmremovevertexpose_free ( URMREMOVEVERTEXPOSE *rvp ) {
    if ( rvp->pos ) free ( rvp->pos );

    list_free ( &rvp->lver, NULL );

    free ( rvp );
}

/******************************************************************************/
static void removeVertexPose_free ( void *data, uint32_t commit ) {
    URMREMOVEVERTEXPOSE *rvp = ( URMREMOVEVERTEXPOSE * ) data;

    urmremovevertexpose_free ( rvp );
}

/******************************************************************************/
static void removeVertexPose_undo ( G3DURMANAGER *urm, 
                                    void         *data, 
                                    uint64_t      engine_flags ) {
    URMREMOVEVERTEXPOSE *rvp = ( URMREMOVEVERTEXPOSE * ) data;

    if ( ((G3DOBJECT*)rvp->mpr)->parent->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)rvp->mpr)->parent;
        LIST *ltmpver = rvp->lver;
        G3DVECTOR *pos = rvp->pos;

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            g3dmorpher_addVertexPose ( rvp->mpr, 
                                       ver, 
                                       rvp->mpose,
                                       pos++ );

            g3dmesh_selectVertex ( mes, ver );

            ltmpver = ltmpver->next;
        }
    }

    g3dscene_updatePivot ( rvp->sce, engine_flags );
}

/******************************************************************************/
static void removeVertexPose_redo ( G3DURMANAGER *urm, 
                                    void         *data, 
                                    uint64_t      engine_flags ) {
    URMREMOVEVERTEXPOSE *rvp = ( URMREMOVEVERTEXPOSE * ) data;

    if ( ((G3DOBJECT*)rvp->mpr)->parent->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)rvp->mpr)->parent;
        LIST *ltmpver = rvp->lver;

        while ( ltmpver ) {
            G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

            g3dmorpher_removeVertexPose ( rvp->mpr,
                                          ver,
                                          rvp->mpose );

            g3dmesh_unselectVertex ( mes, ver );

            ltmpver = ltmpver->next;
        }
    }

    g3dscene_updatePivot ( rvp->sce, engine_flags );
}

/******************************************************************************/
void g3durm_morpher_removeVertexPose ( G3DURMANAGER       *urm,
                                       G3DSCENE           *sce,
                                       G3DMORPHER         *mpr,
                                       G3DMORPHERMESHPOSE *mpose,
                                       uint32_t            engine_flags,
                                       uint32_t            return_flags ) {
    LIST *lver = g3dmorpher_getMeshPoseSelectedVertices ( mpr, mpose );
    URMREMOVEVERTEXPOSE *rvp;

    /*** skip filling the undo/redo stack if there is nothing to undo ***/
    if ( lver ) {
        rvp = urmremovevertexpose_new ( sce, 
                                        mpr,
                                        mpose,
                                        lver );

        removeVertexPose_redo ( urm, rvp, engine_flags );

        g3durmanager_push ( urm, removeVertexPose_undo,
                                 removeVertexPose_redo,
                                 removeVertexPose_free, rvp, return_flags );
    }
}

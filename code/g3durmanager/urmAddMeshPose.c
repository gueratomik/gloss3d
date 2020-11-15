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

typedef struct _URMADDMESHPOSE {
    G3DSCENE             *sce;
    G3DMORPHER           *mpr;
    G3DMORPHERMESHPOSE   *mpose;
    /*** backed up data ***/
    G3DMORPHERVERTEXPOSE *vpose;
    uint32_t              maxVerCount;
} URMADDMESHPOSE;

/******************************************************************************/
static URMADDMESHPOSE *urmaddmeshpose_new ( G3DSCENE           *sce,
                                            G3DMORPHER         *mpr,
                                            G3DMORPHERMESHPOSE *mpose ) {
    uint32_t structsize = sizeof ( URMADDMESHPOSE );

    URMADDMESHPOSE *amp = ( URMADDMESHPOSE * ) calloc ( 0x01, structsize );

    if ( amp == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    amp->sce   = sce;
    amp->mpr   = mpr;
    amp->mpose = mpose;

    /*** this is needed to undo meshpose deletion, not the creation because ***/
    /*** the meshpose is empty during the creation anyways ***/
    amp->maxVerCount = mpose->maxVerCount;

    if ( amp->maxVerCount ) {
        uint32_t ssize = sizeof ( G3DMORPHERVERTEXPOSE );

        amp->vpose = ( G3DMORPHERVERTEXPOSE * ) calloc ( amp->maxVerCount, 
                                                         ssize );

        memcpy ( amp->vpose, mpose->vpose, ssize * amp->maxVerCount );
    }

    return amp;
}

/******************************************************************************/
static void urmaddmeshpose_free ( URMADDMESHPOSE *amp ) {
    if ( amp->vpose ) free ( amp->vpose );

    free ( amp );
}

/******************************************************************************/
static void addMeshPose_free ( void *data, uint32_t commit ) {
    URMADDMESHPOSE *amp = ( URMADDMESHPOSE * ) data;

    if ( commit ) {

    } else {
        g3dmorphermeshpose_free ( amp->mpose );
    }

    urmaddmeshpose_free ( amp );
}

/******************************************************************************/
static void removeMeshPose_free ( void *data, uint32_t commit ) {
    URMADDMESHPOSE *amp = ( URMADDMESHPOSE * ) data;

    if ( commit ) {
        g3dmorphermeshpose_free ( amp->mpose );
    }

    urmaddmeshpose_free ( amp );
}

/******************************************************************************/
static void addMeshPose_undo ( G3DURMANAGER *urm, 
                               void         *data, 
                               uint64_t      engine_flags ) {
    URMADDMESHPOSE *amp = ( URMADDMESHPOSE * ) data;

    g3dmorpher_removeMeshPose ( amp->mpr, 
                                amp->mpose );

    g3dscene_updatePivot ( amp->sce, engine_flags );
}

/******************************************************************************/
static void addMeshPose_redo ( G3DURMANAGER *urm, 
                               void         *data, 
                               uint64_t      engine_flags ) {
    URMADDMESHPOSE *amp = ( URMADDMESHPOSE * ) data;

    if ( amp->maxVerCount ) {
        uint32_t ssize = sizeof ( G3DMORPHERVERTEXPOSE );

        memcpy ( amp->mpose->vpose, 
                 amp->vpose, ssize * amp->maxVerCount );
    }

    /*** adjust the array in case it grew in the meantime ***/
    g3dmorphermeshpose_realloc ( amp->mpose, amp->mpr->verID );

    g3dmorpher_addMeshPose ( amp->mpr, 
                             amp->mpose,
                             amp->mpose->slotID );

    g3dscene_updatePivot ( amp->sce, engine_flags );
}

/******************************************************************************/
void g3durm_morpher_createMeshPose ( G3DURMANAGER *urm,
                                     G3DSCENE     *sce,
                                     G3DMORPHER   *mpr,
                                     uint32_t      engine_flags,
                                     uint32_t      return_flags ) {
    G3DMORPHERMESHPOSE *mpose = g3dmorpher_createMeshPose ( mpr, "Pose" );
    URMADDMESHPOSE *amp;

    g3dscene_updatePivot ( sce, engine_flags );

    amp = urmaddmeshpose_new ( sce, mpr, mpose );

    g3durmanager_push ( urm, addMeshPose_undo,
                             addMeshPose_redo,
                             addMeshPose_free, amp, return_flags );
}

/******************************************************************************/
void g3durm_morpher_removeMeshPose ( G3DURMANAGER       *urm,
                                     G3DSCENE           *sce,
                                     G3DMORPHER         *mpr,
                                     G3DMORPHERMESHPOSE *mpose,
                                     uint32_t            engine_flags,
                                     uint32_t            return_flags ) {
    URMADDMESHPOSE *amp;

    amp = urmaddmeshpose_new ( sce, mpr, mpose );

    g3dmorpher_removeMeshPose ( mpr, mpose );

    g3dscene_updatePivot ( sce, engine_flags );

    /*** note the reversed order of undo / redo callbacks & the free() func ***/
    g3durmanager_push ( urm, addMeshPose_redo,
                             addMeshPose_undo,
                             removeMeshPose_free, amp, return_flags );
}

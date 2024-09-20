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

typedef struct _URMSELECTMESHPOSE {
    G3DSCENE           *sce;
    G3DMORPHER         *mpr;
    G3DMORPHERMESHPOSE *oldmpose;
    G3DMORPHERMESHPOSE *newmpose;
} URMSELECTMESHPOSE;

/******************************************************************************/
static URMSELECTMESHPOSE *urmselectmeshpose_new ( G3DSCENE      *sce,
                                                  G3DMORPHER    *mpr,
                                             G3DMORPHERMESHPOSE *newmpose ) {
    uint32_t ssize = sizeof ( URMSELECTMESHPOSE );

    URMSELECTMESHPOSE *smp = ( URMSELECTMESHPOSE * ) calloc ( 0x01, ssize );

    if ( smp == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    smp->sce      = sce;
    smp->mpr      = mpr;
    smp->oldmpose = mpr->selectedPose;
    smp->newmpose = newmpose;

    return smp;
}

/******************************************************************************/
static void urmselectmeshpose_free ( URMSELECTMESHPOSE *smp ) {
    free ( smp );
}

/******************************************************************************/
static void selectMeshPose_free ( void *data, uint32_t commit ) {
    URMSELECTMESHPOSE *smp = ( URMSELECTMESHPOSE * ) data;

    urmselectmeshpose_free ( smp );
}

/******************************************************************************/
static void selectMeshPose_undo ( G3DURMANAGER *urm, 
                                    void         *data, 
                                    uint64_t      engine_flags ) {
    URMSELECTMESHPOSE *smp = ( URMSELECTMESHPOSE * ) data;

    g3dmorpher_selectMeshPose ( smp->mpr, smp->oldmpose );

    g3dscene_updatePivot ( smp->sce, engine_flags );
}

/******************************************************************************/
static void selectMeshPose_redo ( G3DURMANAGER *urm, 
                                    void         *data, 
                                    uint64_t      engine_flags ) {
    URMSELECTMESHPOSE *smp = ( URMSELECTMESHPOSE * ) data;

    g3dmorpher_selectMeshPose ( smp->mpr, smp->newmpose );

    g3dscene_updatePivot ( smp->sce, engine_flags );
}

/******************************************************************************/
void g3durm_morpher_selectMeshPose ( G3DURMANAGER       *urm,
                                     G3DSCENE           *sce,
                                     G3DMORPHER         *mpr,
                                     G3DMORPHERMESHPOSE *newmpose,
                                     uint32_t            engine_flags,
                                     uint32_t            return_flags ) {
    URMSELECTMESHPOSE *smp;

    if ( newmpose ) {
        smp = urmselectmeshpose_new ( sce, 
                                      mpr,
                                      newmpose );

        g3dmorpher_selectMeshPose ( mpr, newmpose );

        g3dscene_updatePivot ( sce, engine_flags );

        g3durmanager_push ( urm, selectMeshPose_undo,
                                 selectMeshPose_redo,
                                 selectMeshPose_free, smp, return_flags );
    }
}

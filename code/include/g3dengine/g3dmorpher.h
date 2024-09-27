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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/

/**
 * @file
 */

/******************************************************************************/
#ifndef _G3DMORPHER_H_
#define _G3DMORPHER_H_

#ifdef __cplusplus
extern "C" {
#endif


#define MAXMESHPOSESLOT 64 /*** uint64_t bitfield ***/

#define MESHPOSESELECTED ( 1      )

typedef struct _G3DMORPHERVTABLE G3DMORPHERVTABLE;

/******************************************************************************/
typedef struct _G3DMORPHERVERTEXPOSE {
    G3DVECTOR3F pos;
    G3DVECTOR3F nor;
    uint32_t  enabled;
} G3DMORPHERVERTEXPOSE;

/******************************************************************************/
typedef struct _G3DMORPHERMESHPOSE {
    uint32_t              flags;
    uint32_t              id;
    char                 *name;
    G3DMORPHERVERTEXPOSE *vpose;
    uint64_t              slotID;
    uint32_t              maxVertexCount;
    uint32_t              vertexCount;
} G3DMORPHERMESHPOSE;

/**
 * @struct G3DMORPHER
 * @brief A modifier to revolve a spline
 */
typedef struct _G3DMORPHER {
    G3DMODIFIER         mod;  /*** G3DMORPHER inherits G3DMODIFIER        ***/
    uint32_t            extensionName;
    LIST               *vertexList;
    uint32_t            vertexCount;
    uint32_t            verID;
    LIST               *poseList;
    G3DMORPHERMESHPOSE *selectedPose;
    uint64_t            poseSlots;
} G3DMORPHER;

G3DMORPHERMESHPOSE *g3dmorphermeshpose_new ( uint32_t nbver, 
                                             char    *name );
void g3dmorpher_selectMeshPoseByRank ( G3DMORPHER *mpr,
                                       uint32_t    rank );
G3DMORPHERVERTEXPOSE *g3dmorpher_getVertexPose ( G3DMORPHER         *mpr,
                                                 G3DVERTEX          *ver,
                                                 G3DMORPHERMESHPOSE *mpose,
                                                 uint32_t           *nbpose );
G3DMORPHER *g3dmorpher_new ( uint32_t id, 
                             char    *name, 
                             uint64_t engine_flags );
void g3dmorpher_init ( G3DMORPHER       *mpr, 
                       uint32_t          id, 
                       char             *name, 
                       uint64_t          engine_flags,
                       G3DMORPHERVTABLE *vtable );
G3DMORPHERMESHPOSE *g3dmorpher_createMeshPose ( G3DMORPHER *mpr, 
                                                char       *name );
void g3dmorpher_addMeshPose ( G3DMORPHER         *mpr, 
                              G3DMORPHERMESHPOSE *mpose,
                              uint64_t            slotBit );
void g3dmorpher_removeMeshPose ( G3DMORPHER         *mpr, 
                                 G3DMORPHERMESHPOSE *mpose );
G3DMORPHERMESHPOSE *g3dmorpher_getMeshPoseByRank ( G3DMORPHER *mpr,
                                                   uint32_t    rank );
void g3dmorpher_selectMeshPose ( G3DMORPHER         *mpr,
                                 G3DMORPHERMESHPOSE *mpose );
void g3dmorphermeshpose_unsetSelected ( G3DMORPHERMESHPOSE *mpose );
void g3dmorphermeshpose_setSelected ( G3DMORPHERMESHPOSE *mpose );
void g3dmorpher_addVertexPose ( G3DMORPHER         *mpr,
                                G3DVERTEX          *ver,
                                G3DMORPHERMESHPOSE *mpose,
                                G3DVECTOR3F          *vpos );
void g3dmorpher_removeVertexPose ( G3DMORPHER         *mpr,
                                   G3DVERTEX          *ver,
                                   G3DMORPHERMESHPOSE *mpose );

G3DMORPHERMESHPOSE *g3dmorpher_getSelectedMeshPose ( G3DMORPHER *mpr );
uint32_t g3dmorphermeshpose_getVertexPoseCount ( G3DMORPHERMESHPOSE *mpose );
void g3dmorpher_getVertexResetPosition ( G3DMORPHER *mpr,
                                         G3DVERTEX  *ver,
                                         G3DVECTOR3F  *pos );
void g3dmorpher_setVertexResetPosition ( G3DMORPHER *mpr,
                                         G3DVERTEX  *ver,
                                         G3DVECTOR3F  *pos );
int32_t g3dmorpher_getAvailableSlot ( G3DMORPHER *mpr );

void g3dmorpherkey_enableMeshPose ( G3DKEY  *key,
                                    uint32_t slotID );
void g3dmorpherkey_disableMeshPose ( G3DKEY  *key,
                                     uint32_t slotID );
void g3dmorpherkey_setMeshPoseRate ( G3DKEY  *key,
                                     uint32_t slotID,
                                     float    rate  );
float g3dmorpherkey_getMeshPoseRate ( G3DKEY  *key,
                                     uint32_t slotID );
uint32_t g3dmorpherkey_isMeshPoseEnabled ( G3DKEY  *key,
                                           uint32_t slotID );
G3DVECTOR3F *g3dmorpher_getMeshPoseArrayFromList ( G3DMORPHER         *mpr,
                                                 G3DMORPHERMESHPOSE *mpose,
                                                 LIST               *lver );
LIST *g3dmorpher_getMeshPoseSelectedVertices ( G3DMORPHER *mpr,
                                               G3DMORPHERMESHPOSE *mpose );
LIST *g3dmorpher_getMeshPoseVertices ( G3DMORPHER         *mpr,
                                       G3DMORPHERMESHPOSE *mpose );
G3DMORPHERMESHPOSE *g3dmorphermeshpose_copy ( G3DMORPHERMESHPOSE *mpose );
G3DMORPHERMESHPOSE *g3dmorpher_copyMeshPose ( G3DMORPHER         *mpr,
                                              G3DMORPHERMESHPOSE *mpose );
void g3dmorpher_selectMeshVerticesFromPose ( G3DMORPHER         *mpr,
                                             G3DMORPHERMESHPOSE *mpose );
void g3dmorpher_getAveragePositionFromSelectedVertices ( G3DMORPHER *mpr,
                                                         G3DVECTOR3F  *pos );
void g3dmorpher_optimize ( G3DMORPHER *mpr );
void g3dmorpher_restore ( G3DMORPHER *mpr );
void g3dmorphermeshpose_free ( G3DMORPHERMESHPOSE *mpose );
void g3dmorphermeshpose_realloc ( G3DMORPHERMESHPOSE *mpose,
                                  uint32_t            maxVerCount );

#ifdef __cplusplus
}
#endif

#endif

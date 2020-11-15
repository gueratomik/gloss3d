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
#include <g3dexportv2.h>

/******************************************************************************/
static uint32_t g3dexportmorpher_meshPoseName ( G3DEXPORTDATA      *ged,
                                                G3DMORPHERMESHPOSE *mpose,
                                                uint32_t            flags,
                                                FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( mpose->name, strlen ( mpose->name ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmorpher_meshPoseSlotID ( G3DEXPORTDATA      *ged,
                                                  G3DMORPHERMESHPOSE *mpose,
                                                  uint32_t            flags,
                                                  FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &mpose->slotID, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmorpher_meshPoseGeometry ( G3DEXPORTDATA      *ged,
                                                    G3DMORPHERMESHPOSE *mpose,
                                                    uint32_t            flags,
                                                    FILE               *fdst ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) ged->currentObject;
    LIST *ltmpver = mpr->lver;
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &mpose->nbver, fdst );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DMORPHERVERTEXPOSE *vpose = g3dmorpher_getVertexPose ( mpr, 
                                                                 ver,
                                                                 mpose,
                                                                 NULL );

        /*** vpose can be null if vertex pos is not enabled in that pose ***/
        if ( vpose ) {
            size += g3dexport_fwritel ( &ver->id, fdst );
            size += g3dexport_fwritef ( &vpose->pos.x, fdst );
            size += g3dexport_fwritef ( &vpose->pos.y, fdst );
            size += g3dexport_fwritef ( &vpose->pos.z, fdst );
        }

        ltmpver = ltmpver->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmorpher_meshPoseEntry ( G3DEXPORTDATA      *ged,
                                                 G3DMORPHERMESHPOSE *mpose,
                                                 uint32_t            flags,
                                                 FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_MORPHER_MESHPOSE_NAME,
                                   g3dexportmorpher_meshPoseName,
                                   ged,
                                   mpose,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_MORPHER_MESHPOSE_SLOT_ID,
                                   g3dexportmorpher_meshPoseSlotID,
                                   ged,
                                   mpose,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_MORPHER_MESHPOSE_GEOMETRY,
                                   g3dexportmorpher_meshPoseGeometry,
                                   ged,
                                   mpose,
                                   0xFFFFFFFF,
                                   fdst );


    return size;
}

/******************************************************************************/
static uint32_t g3dexportmorpher_meshPoses ( G3DEXPORTDATA *ged,
                                             G3DMORPHER    *mpr,
                                             uint32_t       flags,
                                             FILE          *fdst ) {
    LIST *ltmpmpose = mpr->lmpose;
    uint32_t poseID = 0x00;
    uint32_t size = 0x00;

    while ( ltmpmpose ) {
        G3DMORPHERMESHPOSE *mpose = ltmpmpose->data;

        mpose->id = poseID++;

        size += g3dexport_writeChunk ( SIG_OBJECT_MORPHER_MESHPOSE_ENTRY,
                                       g3dexportmorpher_meshPoseEntry,
                                       ged,
                                       mpose,
                                       0xFFFFFFFF,
                                       fdst );

        ltmpmpose = ltmpmpose->next;
    }


    return size;
}

/******************************************************************************/
static uint32_t g3dexportmorpher_resetPositions ( G3DEXPORTDATA *ged,
                                                  G3DMORPHER    *mpr,
                                                  uint32_t       flags,
                                                  FILE          *fdst ) {
    LIST *ltmpver = mpr->lver;
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &mpr->nbver, fdst );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DVECTOR resetPosition;

        g3dmorpher_getVertexResetPosition ( mpr, ver, &resetPosition );

        size += g3dexport_fwritel ( &ver->id        , fdst );
        size += g3dexport_fwritef ( &resetPosition.x, fdst );
        size += g3dexport_fwritef ( &resetPosition.y, fdst );
        size += g3dexport_fwritef ( &resetPosition.z, fdst );

        ltmpver = ltmpver->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmorpher_vertexCount ( G3DEXPORTDATA *ged,
                                               G3DMORPHER    *mpr,
                                               uint32_t       flags,
                                               FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &mpr->nbver, fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportmorpher ( G3DEXPORTDATA *ged, 
                            G3DMORPHER    *mpr, 
                            uint32_t       flags, 
                            FILE          *fdst ) {
    LIST *ltmpmpose = mpr->lmpose;
    uint32_t size = 0x00;

    if ( mpr->lmpose ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_MORPHER_VERTEX_COUNT,
                                       g3dexportmorpher_vertexCount,
                                       ged,
                                       mpr,
                                       0xFFFFFFFF,
                                       fdst );

        size += g3dexport_writeChunk ( SIG_OBJECT_MORPHER_MESHPOSES,
                                       g3dexportmorpher_meshPoses,
                                       ged,
                                       mpr,
                                       0xFFFFFFFF,
                                       fdst );

        size += g3dexport_writeChunk ( SIG_OBJECT_MORPHER_RESETPOSITIONS,
                                       g3dexportmorpher_resetPositions,
                                       ged,
                                       mpr,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

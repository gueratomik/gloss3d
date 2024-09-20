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
#include <g3dexportv3.h>

/******************************************************************************/
static uint32_t g3dexportv3morpher_meshPoseName ( G3DEXPORTV3DATA      *ged,
                                                G3DMORPHERMESHPOSE *mpose,
                                                uint32_t            flags,
                                                FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwrite ( mpose->name, strlen ( mpose->name ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3morpher_meshPoseSlotID64 ( G3DEXPORTV3DATA    *ged,
                                                      G3DMORPHERMESHPOSE *mpose,
                                                      uint32_t            flags,
                                                      FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritell ( &mpose->slotID, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3morpher_meshPoseGeometry ( G3DEXPORTV3DATA      *ged,
                                                    G3DMORPHERMESHPOSE *mpose,
                                                    uint32_t            flags,
                                                    FILE               *fdst ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) ged->currentObject;
    LIST *ltmpver = mpr->vertexList;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &mpose->vertexCount, fdst );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DMORPHERVERTEXPOSE *vpose = g3dmorpher_getVertexPose ( mpr, 
                                                                 ver,
                                                                 mpose,
                                                                 NULL );

        /*** vpose can be null if vertex pos is not enabled in that pose ***/
        if ( vpose ) {
            size += g3dexportv3_fwritel ( &ver->id, fdst );
            size += g3dexportv3_fwritef ( &vpose->pos.x, fdst );
            size += g3dexportv3_fwritef ( &vpose->pos.y, fdst );
            size += g3dexportv3_fwritef ( &vpose->pos.z, fdst );
        }

        ltmpver = ltmpver->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3morpher_meshPoseEntry ( G3DEXPORTV3DATA      *ged,
                                                 G3DMORPHERMESHPOSE *mpose,
                                                 uint32_t            flags,
                                                 FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_MORPHER_MESHPOSE_NAME,
                   EXPORTV3_CALLBACK(g3dexportv3morpher_meshPoseName),
                                     ged,
                                     mpose,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_MORPHER_MESHPOSE_SLOT_ID64,
                   EXPORTV3_CALLBACK(g3dexportv3morpher_meshPoseSlotID64),
                                     ged,
                                     mpose,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_MORPHER_MESHPOSE_GEOMETRY,
                   EXPORTV3_CALLBACK(g3dexportv3morpher_meshPoseGeometry),
                                     ged,
                                     mpose,
                                     0xFFFFFFFF,
                                     fdst );


    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3morpher_meshPoses ( G3DEXPORTV3DATA *ged,
                                             G3DMORPHER    *mpr,
                                             uint32_t       flags,
                                             FILE          *fdst ) {
    LIST *ltmpmpose = mpr->poseList;
    uint32_t poseID = 0x00;
    uint32_t size = 0x00;

    while ( ltmpmpose ) {
        G3DMORPHERMESHPOSE *mpose = ltmpmpose->data;

        mpose->id = poseID++;

        size += g3dexportv3_writeChunk ( SIG_OBJECT_MORPHER_MESHPOSE_ENTRY,
                       EXPORTV3_CALLBACK(g3dexportv3morpher_meshPoseEntry),
                                         ged,
                                         mpose,
                                         0xFFFFFFFF,
                                         fdst );

        ltmpmpose = ltmpmpose->next;
    }


    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3morpher_vertexCount ( G3DEXPORTV3DATA *ged,
                                               G3DMORPHER    *mpr,
                                               uint32_t       flags,
                                               FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &mpr->vertexCount, fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportv3morpher ( G3DEXPORTV3DATA *ged, 
                            G3DMORPHER    *mpr, 
                            uint32_t       flags, 
                            FILE          *fdst ) {
    LIST *ltmpmpose = mpr->poseList;
    uint32_t size = 0x00;

    if ( mpr->poseList ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_MORPHER_VERTEX_COUNT,
                       EXPORTV3_CALLBACK(g3dexportv3morpher_vertexCount),
                                         ged,
                                         mpr,
                                         0xFFFFFFFF,
                                         fdst );

        size += g3dexportv3_writeChunk ( SIG_OBJECT_MORPHER_MESHPOSES,
                       EXPORTV3_CALLBACK(g3dexportv3morpher_meshPoses),
                                         ged,
                                         mpr,
                                         0xFFFFFFFF,
                                         fdst );
    }

    return size;
}

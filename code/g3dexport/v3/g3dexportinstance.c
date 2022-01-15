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
static uint32_t g3dexportv3instance_reference ( G3DEXPORTV3DATA *ged, 
                                                G3DINSTANCE     *ins, 
                                                uint32_t         flags, 
                                                FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &ins->ref->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3instance_mirroringPlane ( G3DEXPORTV3DATA *ged, 
                                                     G3DINSTANCE     *ins, 
                                                     uint32_t         flags, 
                                                     FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &ins->orientation, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3instance_mirrored ( G3DEXPORTV3DATA *ged, 
                                               G3DINSTANCE     *ins, 
                                               uint32_t         flags, 
                                               FILE            *fdst ) {
    uint32_t size = 0x00;
    uint32_t mirrored = ( ((G3DOBJECT*)ins)->flags & INSTANCEMIRRORED ) ? 0x01 : 
                                                                          0x00;


    size += g3dexportv3_fwritel ( &mirrored, fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportv3instance ( G3DEXPORTV3DATA *ged, 
                               G3DINSTANCE     *ins, 
                               uint32_t         flags, 
                               FILE            *fdst ) {
    uint32_t size = 0x00;

    if ( g3dscene_isObjectReferred ( ins->sce, ins->ref ) ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_INSTANCE_REFERENCE,
                       EXPORTV3_CALLBACK(g3dexportv3instance_reference),
                                         ged,
                                         ins,
                                         0xFFFFFFFF,
                                         fdst );
    }

    size += g3dexportv3_writeChunk ( SIG_OBJECT_INSTANCE_MIRRORINGPLANE,
                   EXPORTV3_CALLBACK(g3dexportv3instance_mirroringPlane),
                                     ged,
                                     ins,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_INSTANCE_MIRRORED,
                   EXPORTV3_CALLBACK(g3dexportv3instance_mirrored),
                                     ged,
                                     ins,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

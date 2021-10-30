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
static uint32_t g3dexportv3subdivider_sync ( G3DEXPORTV3DATA *ged, 
                                             G3DSUBDIVIDER   *sdr, 
                                             uint32_t         flags, 
                                             FILE            *fdst ) {
    uint32_t sync = ( ((G3DOBJECT*)sdr)->flags & SYNCLEVELS ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &sync, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3subdivider_level ( G3DEXPORTV3DATA *ged, 
                                              G3DSUBDIVIDER   *sdr, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &sdr->subdiv_preview, fdst );
    size += g3dexportv3_fwritel ( &sdr->subdiv_render , fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportv3subdivider ( G3DEXPORTV3DATA *ged, 
                               G3DSUBDIVIDER *sdr, 
                               uint32_t       flags, 
                               FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_SUBDIVIDER_SYNC,
                                     g3dexportv3subdivider_sync,
                                     ged,
                                     sdr,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_SUBDIVIDER_LEVEL,
                                   g3dexportv3subdivider_level,
                                   ged,
                                   sdr,
                                   0xFFFFFFFF,
                                   fdst );



    return size;
}

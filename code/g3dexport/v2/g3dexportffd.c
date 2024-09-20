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
static uint32_t g3dexportv2ffd_uvw ( G3DEXPORTV2DATA *ged, 
                                   G3DFFD        *ffd, 
                                   uint32_t       flags, 
                                   FILE          *fdst ) {
    uint32_t nbpnt = ((G3DMESH*)ffd)->vertexCount;
    uint32_t size = 0x00;
    uint32_t i;

    for ( i = 0x00; i < nbpnt; i++ ) {
        G3DVERTEX *pnt = &ffd->pnt[i];

        size += g3dexportv2_fwritef ( &pnt->pos.x, fdst );
        size += g3dexportv2_fwritef ( &pnt->pos.y, fdst );
        size += g3dexportv2_fwritef ( &pnt->pos.z, fdst );
        /*** normal vector stores the local UVW coordinates ***/
        size += g3dexportv2_fwritef ( &pnt->nor.x, fdst );
        size += g3dexportv2_fwritef ( &pnt->nor.y, fdst );
        size += g3dexportv2_fwritef ( &pnt->nor.z, fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2ffd_geometry ( G3DEXPORTV2DATA *ged, 
                                        G3DFFD        *ffd, 
                                        uint32_t       flags, 
                                        FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_fwritel ( &ffd->nbx, fdst );
    size += g3dexportv2_fwritel ( &ffd->nby, fdst );
    size += g3dexportv2_fwritel ( &ffd->nbz, fdst );

    size += g3dexportv2_fwritef ( &ffd->radx, fdst );
    size += g3dexportv2_fwritef ( &ffd->rady, fdst );
    size += g3dexportv2_fwritef ( &ffd->radz, fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportv2ffd ( G3DEXPORTV2DATA *ged, 
                        G3DFFD        *ffd, 
                        uint32_t       flags, 
                        FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_OBJECT_FFD_GEOMETRY,
                   EXPORTV2_CALLBACK(g3dexportv2ffd_geometry),
                                     ged,
                                     ffd,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv2_writeChunk ( SIG_OBJECT_FFD_UVW,
                   EXPORTV2_CALLBACK(g3dexportv2ffd_uvw),
                                     ged,
                                     ffd,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

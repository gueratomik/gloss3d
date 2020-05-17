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
static uint32_t g3dexportsplinerevolver_geometry ( G3DEXPORTDATA     *ged, 
                                                   G3DSPLINEREVOLVER *srv, 
                                                   uint32_t          flags, 
                                                   FILE             *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &srv->nbsteps, fdst );
    size += g3dexport_fwritel ( &srv->nbdivis, fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportsplinerevolver ( G3DEXPORTDATA    *ged, 
                                   G3DSPLINEREVOLVER *srv, 
                                   uint32_t          flags, 
                                   FILE             *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_SPLINEREVOLVER_GEOMETRY,
                                   g3dexportsplinerevolver_geometry,
                                   ged,
                                   srv,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

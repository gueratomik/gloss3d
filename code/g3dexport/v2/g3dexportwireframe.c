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
static uint32_t g3dexportwireframe_geometry ( G3DEXPORTDATA *ged, 
                                              G3DWIREFRAME  *wfm, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &wfm->thickness, fdst );
    size += g3dexport_fwritef ( &wfm->aperture , fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportwireframe_algo  ( G3DEXPORTDATA *ged, 
                                           G3DWIREFRAME  *wfm, 
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    /*** Note: not implemented yet **/
    uint32_t size = 0x00;
    uint32_t algo = 0x00;

    size += g3dexport_fwritel ( &algo, fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportwireframe ( G3DEXPORTDATA *ged, 
                              G3DWIREFRAME  *wfm, 
                              uint32_t       flags, 
                              FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_WIREFRAME_ALGO,
                                   g3dexportwireframe_algo,
                                   ged,
                                   wfm,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_WIREFRAME_GEOMETRY,
                                   g3dexportwireframe_geometry,
                                   ged,
                                   wfm,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

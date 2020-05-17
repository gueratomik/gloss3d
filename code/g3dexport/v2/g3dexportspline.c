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
static uint32_t g3dexportspline_geometrySegments ( G3DEXPORTDATA*ged, 
                                                   G3DSPLINE    *spl, 
                                                   uint32_t      flags, 
                                                   FILE         *fdst ) {
    LIST *ltmpseg = spl->curve->lseg;
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &spl->curve->nbseg, fdst );

    while ( ltmpseg ) {
        G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;

        size += g3dexport_fwritel ( &seg->pt[P0IDX]->id, fdst );
        size += g3dexport_fwritel ( &seg->pt[P1IDX]->id, fdst );

        size += g3dexport_fwritef ( &seg->pt[P0HANDLEIDX]->pos.x, fdst );
        size += g3dexport_fwritef ( &seg->pt[P0HANDLEIDX]->pos.y, fdst );
        size += g3dexport_fwritef ( &seg->pt[P0HANDLEIDX]->pos.z, fdst );

        size += g3dexport_fwritef ( &seg->pt[P1HANDLEIDX]->pos.x, fdst );
        size += g3dexport_fwritef ( &seg->pt[P1HANDLEIDX]->pos.y, fdst );
        size += g3dexport_fwritef ( &seg->pt[P1HANDLEIDX]->pos.z, fdst );

        ltmpseg = ltmpseg->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportspline_geometryPoints ( G3DEXPORTDATA*ged, 
                                                 G3DSPLINE    *spl, 
                                                 uint32_t      flags, 
                                                 FILE         *fdst ) {
    LIST *ltmppt = spl->curve->lpt;
    uint32_t size = 0x00;
    uint32_t ptID = 0x00;

    size += g3dexport_fwritel ( &spl->curve->nbpt, fdst );

    while ( ltmppt ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;

        size += g3dexport_fwritef ( &pt->pos.x, fdst );
        size += g3dexport_fwritef ( &pt->pos.y, fdst );
        size += g3dexport_fwritef ( &pt->pos.z, fdst );
        size += g3dexport_fwritel ( &pt->flags, fdst );

        pt->id = ptID++;

        ltmppt = ltmppt->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportspline_geometry ( G3DEXPORTDATA*ged, 
                                           G3DSPLINE    *spl, 
                                           uint32_t      flags, 
                                           FILE         *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_SPLINE_GEOMETRY_POINTS,
                                   g3dexportspline_geometryPoints,
                                   ged,
                                   spl,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_SPLINE_GEOMETRY_SEGMENTS,
                                   g3dexportspline_geometrySegments,
                                   ged,
                                   spl,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportspline ( G3DEXPORTDATA*ged, 
                           G3DSPLINE    *spl, 
                           uint32_t      flags, 
                           FILE         *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_SPLINE_GEOMETRY,
                                   g3dexportspline_geometry,
                                   ged,
                                   spl,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

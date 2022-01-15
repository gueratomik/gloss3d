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
static uint32_t g3dexportv3plane_base ( G3DEXPORTV3DATA *ged, 
                                        G3DPRIMITIVE    *pri, 
                                        uint32_t         flags, 
                                        FILE            *fdst ) {
    PLANEDATASTRUCT *pds = ( PLANEDATASTRUCT * ) pri->data;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pds->radu       , fdst );
    size += g3dexportv3_fwritef ( &pds->radv       , fdst );
    size += g3dexportv3_fwritel ( &pds->nbu        , fdst );
    size += g3dexportv3_fwritel ( &pds->nbv        , fdst );
    size += g3dexportv3_fwritel ( &pds->orientation, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3plane ( G3DEXPORTV3DATA *ged, 
                                  G3DPRIMITIVE    *pri, 
                                  uint32_t         flags, 
                                  FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PRIMITIVE_PLANE_BASE,
                   EXPORTV3_CALLBACK(g3dexportv3plane_base),
                                     ged,
                                     pri,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3tube_base ( G3DEXPORTV3DATA *ged, 
                                       G3DPRIMITIVE    *pri, 
                                       uint32_t         flags, 
                                       FILE            *fdst ) {
    TUBEDATASTRUCT *tds = ( TUBEDATASTRUCT * ) pri->data;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &tds->length     , fdst );
    size += g3dexportv3_fwritef ( &tds->radius     , fdst );
    size += g3dexportv3_fwritel ( &tds->slice      , fdst );
    size += g3dexportv3_fwritel ( &tds->capx       , fdst );
    size += g3dexportv3_fwritel ( &tds->capy       , fdst );
    size += g3dexportv3_fwritef ( &tds->thickness  , fdst );
    size += g3dexportv3_fwritel ( &tds->orientation, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3tube ( G3DEXPORTV3DATA *ged, 
                                  G3DPRIMITIVE    *pri, 
                                  uint32_t         flags, 
                                  FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PRIMITIVE_TUBE_BASE,
                   EXPORTV3_CALLBACK(g3dexportv3tube_base),
                                     ged,
                                     pri,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3cylinder_base ( G3DEXPORTV3DATA *ged, 
                                           G3DPRIMITIVE  *pri, 
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    CYLINDERDATASTRUCT *cds = ( CYLINDERDATASTRUCT * ) pri->data;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef  ( &cds->length     , fdst );
    size += g3dexportv3_fwritef  ( &cds->radius     , fdst );
    size += g3dexportv3_fwritel  ( &cds->slice      , fdst );
    size += g3dexportv3_fwritel  ( &cds->capx       , fdst );
    size += g3dexportv3_fwritel  ( &cds->capy       , fdst );
    size += g3dexportv3_fwritel  ( &cds->closed     , fdst );
    size += g3dexportv3_fwritel  ( &cds->orientation, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3cylinder ( G3DEXPORTV3DATA *ged, 
                                      G3DPRIMITIVE    *pri, 
                                      uint32_t         flags, 
                                      FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PRIMITIVE_CYLINDER_BASE,
                   EXPORTV3_CALLBACK(g3dexportv3cylinder_base),
                                     ged,
                                     pri,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3cube_base ( G3DEXPORTV3DATA *ged, 
                                       G3DPRIMITIVE    *pri, 
                                       uint32_t         flags, 
                                       FILE            *fdst ) {
    CUBEDATASTRUCT *cds = ( CUBEDATASTRUCT * ) pri->data;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &cds->radius, fdst );
    size += g3dexportv3_fwritel ( &cds->nbx   , fdst );
    size += g3dexportv3_fwritel ( &cds->nby   , fdst );
    size += g3dexportv3_fwritel ( &cds->nbz   , fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3cube ( G3DEXPORTV3DATA *ged, 
                                  G3DPRIMITIVE    *pri, 
                                  uint32_t         flags, 
                                  FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PRIMITIVE_CUBE_BASE,
                   EXPORTV3_CALLBACK(g3dexportv3cube_base),
                                     ged,
                                     pri,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3torus_base ( G3DEXPORTV3DATA *ged, 
                                        G3DPRIMITIVE  *pri, 
                                        uint32_t       flags, 
                                        FILE          *fdst ) {
    TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &tds->extrad     , fdst );
    size += g3dexportv3_fwritef ( &tds->intrad     , fdst );
    size += g3dexportv3_fwritel ( &tds->slice      , fdst );
    size += g3dexportv3_fwritel ( &tds->cap        , fdst );
    size += g3dexportv3_fwritel ( &tds->orientation, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3torus ( G3DEXPORTV3DATA *ged, 
                                   G3DPRIMITIVE    *pri, 
                                   uint32_t         flags, 
                                   FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PRIMITIVE_TORUS_BASE,
                    EXPORTV3_CALLBACK(g3dexportv3torus_base),
                                     ged,
                                     pri,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3sphere_base ( G3DEXPORTV3DATA *ged, 
                                         G3DPRIMITIVE    *pri, 
                                         uint32_t         flags, 
                                         FILE            *fdst ) {
    SPHEREDATASTRUCT *sds = ( SPHEREDATASTRUCT * ) pri->data;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &sds->radius, fdst );
    size += g3dexportv3_fwritel ( &sds->slice , fdst );
    size += g3dexportv3_fwritel ( &sds->cap   , fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3sphere ( G3DEXPORTV3DATA *ged, 
                                    G3DPRIMITIVE    *pri, 
                                    uint32_t         flags, 
                                    FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PRIMITIVE_SPHERE_BASE,
                   EXPORTV3_CALLBACK(g3dexportv3sphere_base),
                                     ged,
                                     pri,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportv3primitive ( G3DEXPORTV3DATA *ged, 
                              G3DOBJECT     *obj, 
                              uint32_t       flags, 
                              FILE          *fdst ) {
    uint32_t size = 0x00;

    if ( obj->type == G3DSPHERETYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_PRIMITIVE_SPHERE,
                       EXPORTV3_CALLBACK(g3dexportv3sphere),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DTORUSTYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_PRIMITIVE_TORUS,
                       EXPORTV3_CALLBACK(g3dexportv3torus),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DCUBETYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_PRIMITIVE_CUBE,
                       EXPORTV3_CALLBACK(g3dexportv3cube),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DCYLINDERTYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_PRIMITIVE_CYLINDER,
                       EXPORTV3_CALLBACK(g3dexportv3cylinder),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DTUBETYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_PRIMITIVE_TUBE,
                       EXPORTV3_CALLBACK(g3dexportv3tube),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( obj->type == G3DPLANETYPE ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_PRIMITIVE_PLANE,
                       EXPORTV3_CALLBACK(g3dexportv3plane),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );
    }

    return size;
}

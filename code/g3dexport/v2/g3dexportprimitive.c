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
static uint32_t g3dexportplane ( G3DEXPORTDATA *ged, 
                                 G3DPRIMITIVE  *pri, 
                                 uint32_t       flags, 
                                 FILE          *fdst ) {
    PLANEDATASTRUCT *pds = ( PLANEDATASTRUCT * ) pri->data;
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( &pds->radu       , sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite ( &pds->radv       , sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite ( &pds->nbu        , sizeof ( uint32_t ), 0x01, fdst );
    size += g3dexport_fwrite ( &pds->nbv        , sizeof ( uint32_t ), 0x01, fdst );
    size += g3dexport_fwrite ( &pds->orientation, sizeof ( uint32_t ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportcylinder ( G3DEXPORTDATA *ged, 
                                    G3DPRIMITIVE  *pri, 
                                    uint32_t       flags, 
                                    FILE          *fdst ) {
    CYLINDERDATASTRUCT *cds = ( CYLINDERDATASTRUCT * ) pri->data;
    uint32_t size = 0x00;

    size += g3dexport_fwrite  ( &cds->length     , sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite  ( &cds->radius     , sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite  ( &cds->slice      , sizeof ( uint32_t ), 0x01, fdst );
    size += g3dexport_fwrite  ( &cds->capx       , sizeof ( uint32_t ), 0x01, fdst );
    size += g3dexport_fwrite  ( &cds->capy       , sizeof ( uint32_t ), 0x01, fdst );
    size += g3dexport_fwrite  ( &cds->closed     , sizeof ( uint32_t ), 0x01, fdst );
    size += g3dexport_fwrite  ( &cds->orientation, sizeof ( uint32_t ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportcube ( G3DEXPORTDATA *ged, 
                                G3DPRIMITIVE  *pri, 
                                uint32_t       flags, 
                                FILE          *fdst ) {
    CUBEDATASTRUCT *cds = ( CUBEDATASTRUCT * ) pri->data;
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( &cds->radius, sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite ( &cds->nbx   , sizeof ( uint32_t ), 0x01, fdst );
    size += g3dexport_fwrite ( &cds->nby   , sizeof ( uint32_t ), 0x01, fdst );
    size += g3dexport_fwrite ( &cds->nbz   , sizeof ( uint32_t ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexporttorus ( G3DEXPORTDATA *ged, 
                                 G3DPRIMITIVE  *pri, 
                                 uint32_t       flags, 
                                 FILE          *fdst ) {
    TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( &tds->extrad     , sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite ( &tds->intrad     , sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite ( &tds->slice      , sizeof ( uint32_t ), 0x01, fdst );
    size += g3dexport_fwrite ( &tds->cap        , sizeof ( uint32_t ), 0x01, fdst );
    size += g3dexport_fwrite ( &tds->orientation, sizeof ( uint32_t ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportsphere ( G3DEXPORTDATA *ged, 
                                  G3DPRIMITIVE  *pri, 
                                  uint32_t       flags, 
                                  FILE          *fdst ) {
    SPHEREDATASTRUCT *sds = ( SPHEREDATASTRUCT * ) pri->data;
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( &sds->radius, sizeof ( float    ), 0x01, fdst );
    size += g3dexport_fwrite ( &sds->slice , sizeof ( uint32_t ), 0x01, fdst );
    size += g3dexport_fwrite ( &sds->cap   , sizeof ( uint32_t ), 0x01, fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportprimitive ( G3DEXPORTDATA *ged, 
                              G3DOBJECT     *obj, 
                              uint32_t       flags, 
                              FILE          *fdst ) {
    uint32_t size = 0x00;

    if ( obj->type == G3DSPHERETYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_PRIMITIVE_SPHERE,
                                       g3dexportsphere,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( obj->type == G3DTORUSTYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_PRIMITIVE_TORUS,
                                       g3dexporttorus,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( obj->type == G3DCUBETYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_PRIMITIVE_CUBE,
                                       g3dexportcube,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( obj->type == G3DCYLINDERTYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_PRIMITIVE_CYLINDER,
                                       g3dexportcylinder,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( obj->type == G3DPLANETYPE ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_PRIMITIVE_PLANE,
                                       g3dexportplane,
                                       ged,
                                       obj,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

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
static uint32_t g3dexportv3uvmap_transformation ( G3DEXPORTV3DATA  *ged,
                                                G3DUVMAP       *uvmap,
                                                uint32_t        flags, 
                                                FILE           *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) uvmap;
    uint32_t size = 0x00;
    float w = 0.0f;

    size += g3dexportv3_fwritef ( &obj->pos.x, fdst );
    size += g3dexportv3_fwritef ( &obj->pos.y, fdst );
    size += g3dexportv3_fwritef ( &obj->pos.z, fdst );
    size += g3dexportv3_fwritef ( &w         , fdst );

    size += g3dexportv3_fwritef ( &obj->rot.x, fdst );
    size += g3dexportv3_fwritef ( &obj->rot.y, fdst );
    size += g3dexportv3_fwritef ( &obj->rot.z, fdst );
    size += g3dexportv3_fwritef ( &w         , fdst );

    size += g3dexportv3_fwritef ( &obj->sca.x, fdst );
    size += g3dexportv3_fwritef ( &obj->sca.y, fdst );
    size += g3dexportv3_fwritef ( &obj->sca.z, fdst );
    size += g3dexportv3_fwritef ( &w         , fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3uvmap_UVSets ( G3DEXPORTV3DATA  *ged,
                                        G3DUVMAP       *uvmap,
                                        uint32_t        flags, 
                                        FILE           *fdst ) {
    G3DMESH *mes = ( G3DMESH * ) ged->currentObject;
    LIST *ltmpfac = mes->faceList;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &mes->faceCount, fdst );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DUVSET *uvs = g3dface_getUVSet ( fac, uvmap );
        uint32_t uvflags = uvs->flags & (~(UVSETSELECTED));

        size += g3dexportv3_fwritel ( &uvflags, fdst );
        size += g3dexportv3_fwritel ( &fac->id, fdst );

        size += g3dexportv3_fwritef ( &uvs->veruv[0x00].u, fdst );
        size += g3dexportv3_fwritef ( &uvs->veruv[0x00].v, fdst );
        size += g3dexportv3_fwritef ( &uvs->veruv[0x01].u, fdst );
        size += g3dexportv3_fwritef ( &uvs->veruv[0x01].v, fdst );
        size += g3dexportv3_fwritef ( &uvs->veruv[0x02].u, fdst );
        size += g3dexportv3_fwritef ( &uvs->veruv[0x02].v, fdst );
        size += g3dexportv3_fwritef ( &uvs->veruv[0x03].u, fdst );
        size += g3dexportv3_fwritef ( &uvs->veruv[0x03].v, fdst );

        ltmpfac = ltmpfac->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3uvmap_radius ( G3DEXPORTV3DATA  *ged, 
                                        G3DUVMAP       *uvmap, 
                                        uint32_t        flags, 
                                        FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &uvmap->pln.xradius, fdst );
    size += g3dexportv3_fwritef ( &uvmap->pln.yradius, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3uvmap_flags ( G3DEXPORTV3DATA  *ged, 
                                       G3DUVMAP       *uvmap, 
                                       uint32_t        flags, 
                                       FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &((G3DOBJECT*)uvmap)->flags, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3uvmap_projection ( G3DEXPORTV3DATA  *ged, 
                                            G3DUVMAP       *uvmap, 
                                            uint32_t        flags, 
                                            FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &uvmap->projection, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3uvmap_name ( G3DEXPORTV3DATA  *ged, 
                                      G3DUVMAP       *uvmap, 
                                      uint32_t        flags, 
                                      FILE           *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) uvmap;
    uint32_t size = 0x00;

    size += g3dexportv3_fwrite ( obj->name, strlen ( obj->name ), 0x01, fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportv3uvmap ( G3DEXPORTV3DATA  *ged, 
                          G3DUVMAP       *uvmap, 
                          uint32_t        flags, 
                          FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_UVMAP_PROJECTION,
                   EXPORTV3_CALLBACK(g3dexportv3uvmap_projection),
                                     ged,
                                     uvmap,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_UVMAP_NAME,
                   EXPORTV3_CALLBACK(g3dexportv3uvmap_name),
                                     ged,
                                     uvmap,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_UVMAP_FLAGS,
                   EXPORTV3_CALLBACK(g3dexportv3uvmap_flags),
                                     ged,
                                     uvmap,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_UVMAP_RADIUS,
                   EXPORTV3_CALLBACK(g3dexportv3uvmap_radius),
                                     ged,
                                     uvmap,
                                     0xFFFFFFFF,
                                     fdst );

    if ( ((G3DOBJECT*)uvmap)->flags & UVMAPFIXED ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_UVMAP_UVSETS,
                       EXPORTV3_CALLBACK(g3dexportv3uvmap_UVSets),
                                         ged,
                                         uvmap,
                                         0xFFFFFFFF,
                                         fdst );
    }

    size += g3dexportv3_writeChunk ( SIG_OBJECT_UVMAP_TRANSFORMATION,
                   EXPORTV3_CALLBACK(g3dexportv3uvmap_transformation),
                                     ged,
                                     uvmap,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}



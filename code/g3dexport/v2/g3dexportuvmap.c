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
static uint32_t g3dexportuvmap_transformation ( G3DEXPORTDATA  *ged,
                                                G3DUVMAP       *uvmap,
                                                uint32_t        flags, 
                                                FILE           *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) uvmap;
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &obj->pos.x, fdst );
    size += g3dexport_fwritef ( &obj->pos.y, fdst );
    size += g3dexport_fwritef ( &obj->pos.z, fdst );
    size += g3dexport_fwritef ( &obj->pos.w, fdst );

    size += g3dexport_fwritef ( &obj->rot.x, fdst );
    size += g3dexport_fwritef ( &obj->rot.y, fdst );
    size += g3dexport_fwritef ( &obj->rot.z, fdst );
    size += g3dexport_fwritef ( &obj->rot.w, fdst );

    size += g3dexport_fwritef ( &obj->sca.x, fdst );
    size += g3dexport_fwritef ( &obj->sca.y, fdst );
    size += g3dexport_fwritef ( &obj->sca.z, fdst );
    size += g3dexport_fwritef ( &obj->sca.w, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportuvmap_material ( G3DEXPORTDATA  *ged,
                                          G3DMATERIAL    *mat,
                                          uint32_t        flags, 
                                          FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &mat->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportuvmap_materials ( G3DEXPORTDATA  *ged,
                                           G3DUVMAP       *uvmap,
                                           uint32_t        flags, 
                                           FILE           *fdst ) {
    LIST *ltmpmat = uvmap->lmat;
    uint32_t size = 0x00;

    while ( ltmpmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpmat->data;

        size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_MATERIAL_ENTRY,
                                       g3dexportuvmap_material,
                                       ged,
                                       mat,
                                       0xFFFFFFFF,
                                       fdst );

        ltmpmat = ltmpmat->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportuvmap_UVSets ( G3DEXPORTDATA  *ged,
                                        G3DUVMAP       *uvmap,
                                        uint32_t        flags, 
                                        FILE           *fdst ) {
    G3DMESH *mes = ( G3DMESH * ) ged->currentObject;
    LIST *ltmpfac = mes->lfac;
    uint32_t size = 0x00;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DUVSET *uvs = g3dface_getUVSet ( fac, uvmap );

        size += g3dexport_fwritel ( &uvs->flags, fdst );
        size += g3dexport_fwritel ( &fac->id, fdst );

        size += g3dexport_fwritef ( &uvs->veruv[0x00].u, fdst );
        size += g3dexport_fwritef ( &uvs->veruv[0x00].v, fdst );
        size += g3dexport_fwritef ( &uvs->veruv[0x01].u, fdst );
        size += g3dexport_fwritef ( &uvs->veruv[0x01].v, fdst );
        size += g3dexport_fwritef ( &uvs->veruv[0x02].u, fdst );
        size += g3dexport_fwritef ( &uvs->veruv[0x02].v, fdst );
        size += g3dexport_fwritef ( &uvs->veruv[0x03].u, fdst );
        size += g3dexport_fwritef ( &uvs->veruv[0x03].v, fdst );

        ltmpfac = ltmpfac->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportuvmap_radius ( G3DEXPORTDATA  *ged, 
                                        G3DUVMAP       *uvmap, 
                                        uint32_t        flags, 
                                        FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &uvmap->pln.xradius, fdst );
    size += g3dexport_fwritef ( &uvmap->pln.yradius, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportuvmap_flags ( G3DEXPORTDATA  *ged, 
                                       G3DUVMAP       *uvmap, 
                                       uint32_t        flags, 
                                       FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &((G3DOBJECT*)uvmap)->flags, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportuvmap_projection ( G3DEXPORTDATA  *ged, 
                                            G3DUVMAP       *uvmap, 
                                            uint32_t        flags, 
                                            FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &uvmap->projection, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportuvmap_name ( G3DEXPORTDATA  *ged, 
                                      G3DUVMAP       *uvmap, 
                                      uint32_t        flags, 
                                      FILE           *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) uvmap;
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( obj->name, strlen ( obj->name ), 0x01, fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportuvmap ( G3DEXPORTDATA  *ged, 
                          G3DUVMAP       *uvmap, 
                          uint32_t        flags, 
                          FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_PROJECTION,
                                   g3dexportuvmap_projection,
                                   ged,
                                   uvmap,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_NAME,
                                   g3dexportuvmap_name,
                                   ged,
                                   uvmap,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_FLAGS,
                                   g3dexportuvmap_flags,
                                   ged,
                                   uvmap,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_RADIUS,
                                   g3dexportuvmap_radius,
                                   ged,
                                   uvmap,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_MATERIALS,
                                   g3dexportuvmap_materials,
                                   ged,
                                   uvmap,
                                   0xFFFFFFFF,
                                   fdst );

    if ( ((G3DOBJECT*)uvmap)->flags & UVMAPFIXED ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_UVSETS,
                                       g3dexportuvmap_UVSets,
                                       ged,
                                       uvmap,
                                       0xFFFFFFFF,
                                       fdst );
    }



    size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_TRANSFORMATION,
                                   g3dexportuvmap_transformation,
                                   ged,
                                   uvmap,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}



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
static uint32_t g3dexportmesh_uvmapTransformation ( G3DEXPORTDATA  *ged,
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
static uint32_t g3dexportmesh_uvmapMaterials ( G3DEXPORTDATA  *ged,
                                               G3DUVMAP       *uvmap,
                                               uint32_t        flags, 
                                               FILE           *fdst ) {
    LIST *ltmpmat = uvmap->lmat;
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &uvmap->nbmat, fdst );

    while ( ltmpmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpmat->data;

        size += g3dexport_fwritel ( &mat->id, fdst );

        ltmpmat = ltmpmat->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_uvmapUVSetEntry ( G3DEXPORTDATA  *ged,
                                                G3DUVSET       *uvs,
                                                uint32_t        flags, 
                                                FILE           *fdst ) {
    G3DFACE *fac = ( G3DFACE * ) ged->currentFace;
    uint32_t size = 0x00;

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

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_uvmapUVSets ( G3DEXPORTDATA  *ged,
                                            G3DUVMAP       *uvmap,
                                            uint32_t        flags, 
                                            FILE           *fdst ) {
    G3DMESH *mes = ( G3DMESH * ) ged->currentObject;
    LIST *ltmpfac = mes->lfac;
    uint32_t size = 0x00;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        G3DUVSET *uvs = g3dface_getUVSet ( fac, uvmap );

        ged->currentFace = fac;

        size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_UVSET_ENTRY,
                                       g3dexportmesh_uvmapUVSetEntry,
                                       ged,
                                       uvs,
                                       0xFFFFFFFF,
                                       fdst );

        ged->currentFace = NULL;

        ltmpfac = ltmpfac->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_uvmapRadius ( G3DEXPORTDATA  *ged, 
                                            G3DUVMAP       *uvmap, 
                                            uint32_t        flags, 
                                            FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &uvmap->pln.xradius, fdst );
    size += g3dexport_fwritef ( &uvmap->pln.yradius, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_uvmapInfo ( G3DEXPORTDATA  *ged, 
                                          G3DUVMAP       *uvmap, 
                                          uint32_t        flags, 
                                          FILE           *fdst ) {
    uint32_t isFixed = ( ((G3DOBJECT*)uvmap)->flags & UVMAPFIXED ) ? 1 : 0;
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &uvmap->projection, fdst );
    size += g3dexport_fwritel ( &isFixed, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_uvmapName ( G3DEXPORTDATA  *ged, 
                                          G3DUVMAP       *uvmap, 
                                          uint32_t        flags, 
                                          FILE           *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) uvmap;
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( obj->name, strlen ( obj->name ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_uvmapEntry ( G3DEXPORTDATA  *ged, 
                                           G3DUVMAP       *uvmap, 
                                           uint32_t        flags, 
                                           FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_NAME,
                                   g3dexportmesh_uvmapName,
                                   ged,
                                   uvmap,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_INFO,
                                   g3dexportmesh_uvmapInfo,
                                   ged,
                                   uvmap,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_RADIUS,
                                   g3dexportmesh_uvmapRadius,
                                   ged,
                                   uvmap,
                                   0xFFFFFFFF,
                                   fdst );

    if ( ((G3DOBJECT*)uvmap)->flags & UVMAPFIXED ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_UVSETS,
                                       g3dexportmesh_uvmapUVSets,
                                       ged,
                                       uvmap,
                                       0xFFFFFFFF,
                                       fdst );
    }

    size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_MATERIALS,
                                   g3dexportmesh_uvmapMaterials,
                                   ged,
                                   uvmap,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_TRANSFORMATION,
                                   g3dexportmesh_uvmapTransformation,
                                   ged,
                                   uvmap,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_uvmaps ( G3DEXPORTDATA  *ged, 
                                       G3DMESH        *mes, 
                                       uint32_t        flags, 
                                       FILE           *fdst ) {
    LIST *ltmpuvmap = mes->luvmap;
    uint32_t size = 0x00;

    while ( ltmpuvmap ) {
        G3DUVMAP *uvmap = ( G3DUVMAP * ) ltmpuvmap->data;

        size += g3dexport_writeChunk ( SIG_OBJECT_UVMAP_ENTRY,
                                       g3dexportmesh_uvmapEntry,
                                       ged,
                                       uvmap,
                                       0xFFFFFFFF,
                                       fdst );

        ltmpuvmap = ltmpuvmap->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_weightgroupWeights ( G3DEXPORTDATA  *ged, 
                                                   G3DWEIGHTGROUP *grp, 
                                                   uint32_t        flags, 
                                                   FILE           *fdst ) {
    LIST *ltmpwei = grp->lwei;
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &grp->nbwei, fdst );

    while ( ltmpwei ) {
        G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmpwei->data;

        size += g3dexport_fwritel ( &wei->ver->id, fdst );
        size += g3dexport_fwritef ( &wei->weight, fdst );

        ltmpwei = ltmpwei->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_weightgroupName ( G3DEXPORTDATA  *ged, 
                                                G3DWEIGHTGROUP *grp, 
                                                uint32_t        flags, 
                                                FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( grp->name, strlen ( grp->name ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_weightgroupEntry ( G3DEXPORTDATA  *ged, 
                                                 G3DWEIGHTGROUP *grp, 
                                                 uint32_t        flags, 
                                                 FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_MESH_WEIGHTGROUP_NAME,
                                   g3dexportmesh_weightgroupName,
                                   ged,
                                   grp,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_MESH_WEIGHTGROUP_WEIGHTS,
                                   g3dexportmesh_weightgroupWeights,
                                   ged,
                                   grp,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_weightgroups ( G3DEXPORTDATA *ged, 
                                             G3DMESH       *mes, 
                                             uint32_t       flags, 
                                             FILE          *fdst ) {
    LIST *ltmpgrp = mes->lgrp;
    uint32_t grpid = 0x00;
    uint32_t size = 0x00;

    while ( ltmpgrp ) {
        G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmpgrp->data;

        grp->id = grpid++; /*** for indexation by skeleton ***/

        size += g3dexport_writeChunk ( SIG_OBJECT_MESH_WEIGHTGROUP_ENTRY,
                                       g3dexportmesh_weightgroupEntry,
                                       ged,
                                       grp,
                                       0xFFFFFFFF,
                                       fdst );

        ltmpgrp = ltmpgrp->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_geometryPolygons ( G3DEXPORTDATA *ged, 
                                                 G3DMESH       *mes, 
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    LIST *ltmpfac = mes->lfac;
    uint32_t size = 0x00;
    uint32_t fid = 0x00;

    size += g3dexport_fwritel ( &mes->nbfac, fdst );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        /*** write twice the last vertex in case of a triangle ***/
        uint32_t idx[0x04] = { 0, 1, 2, ( fac->nbver == 0x03 ) ? 2 : 3 };

        fac->id = fid++;

        size += g3dexport_fwritel ( &fac->ver[idx[0x00]]->id, fdst );
        size += g3dexport_fwritel ( &fac->ver[idx[0x01]]->id, fdst );
        size += g3dexport_fwritel ( &fac->ver[idx[0x02]]->id, fdst );
        size += g3dexport_fwritel ( &fac->ver[idx[0x03]]->id, fdst );

        ltmpfac = ltmpfac->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_geometryVertices ( G3DEXPORTDATA *ged, 
                                                 G3DMESH       *mes, 
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    LIST *ltmpver = mes->lver;
    uint32_t size = 0x00;
    uint32_t vid = 0x00;

    size += g3dexport_fwritel ( &mes->nbver, fdst );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        ver->id = vid++;

        size += g3dexport_fwritef ( &ver->pos.x, fdst );
        size += g3dexport_fwritef ( &ver->pos.y, fdst );
        size += g3dexport_fwritef ( &ver->pos.z, fdst );
        size += g3dexport_fwritef ( &ver->pos.w, fdst );

        ltmpver = ltmpver->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_geometry ( G3DEXPORTDATA *ged, 
                                         G3DMESH       *mes, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;

    if ( mes->lver ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_MESH_GEOMETRY_VERTICES,
                                       g3dexportmesh_geometryVertices,
                                       ged,
                                       mes,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mes->lfac ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_MESH_GEOMETRY_POLYGONS,
                                       g3dexportmesh_geometryPolygons,
                                       ged,
                                       mes,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

/******************************************************************************/
uint32_t g3dexportmesh ( G3DEXPORTDATA *ged, 
                         G3DMESH       *mes, 
                         uint32_t       flags, 
                         FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_MESH_GEOMETRY,
                                   g3dexportmesh_geometry,
                                   ged,
                                   mes,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_MESH_WEIGHTGROUPS,
                                   g3dexportmesh_weightgroups,
                                   ged,
                                   mes,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_UVMAPS,
                                   g3dexportmesh_uvmaps,
                                   ged,
                                   mes,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

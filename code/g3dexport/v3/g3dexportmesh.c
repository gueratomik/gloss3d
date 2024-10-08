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
static uint32_t g3dexportv3mesh_facegroupFaces ( G3DEXPORTV3DATA *ged, 
                                                 G3DFACEGROUP  *grp, 
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    G3DMESH *mes = ( G3DMESH * ) ged->currentObject;
    LIST *ltmpfacpass1 = mes->faceList;
    LIST *ltmpfacpass2 = mes->faceList;
    uint32_t size = 0x00;
    uint32_t faceCount = 0x00;

    while ( ltmpfacpass1 ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfacpass1->data;

        if ( list_seek ( fac->facegroupList, grp ) ) faceCount++;

        ltmpfacpass1 = ltmpfacpass1->next;
    }

    size += g3dexportv3_fwritel ( &faceCount, fdst );



    while ( ltmpfacpass2 ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfacpass2->data;

        if ( list_seek ( fac->facegroupList, grp ) ) {
            size += g3dexportv3_fwritel ( &fac->id, fdst );
        }

        ltmpfacpass2 = ltmpfacpass2->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3mesh_facegroupName ( G3DEXPORTV3DATA *ged, 
                                                G3DFACEGROUP  *grp, 
                                                uint32_t       flags, 
                                                FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwrite ( grp->name, strlen ( grp->name ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3mesh_facegroupEntry ( G3DEXPORTV3DATA *ged, 
                                               G3DFACEGROUP  *grp, 
                                               uint32_t       flags, 
                                               FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_FACEGROUP_NAME,
                   EXPORTV3_CALLBACK(g3dexportv3mesh_facegroupName),
                                     ged,
                                     grp,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_FACEGROUP_FACES,
                   EXPORTV3_CALLBACK(g3dexportv3mesh_facegroupFaces),
                                     ged,
                                     grp,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3mesh_facegroups ( G3DEXPORTV3DATA *ged, 
                                           G3DMESH       *mes, 
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    LIST *ltmpgrp = mes->facegroupList;
    uint32_t grpid = 0x00;
    uint32_t size = 0x00;

    while ( ltmpgrp ) {
        G3DFACEGROUP *grp = ( G3DFACEGROUP * ) ltmpgrp->data;

        grp->id = grpid++; /*** for indexation by textures ***/

        size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_FACEGROUP_ENTRY,
                       EXPORTV3_CALLBACK(g3dexportv3mesh_facegroupEntry),
                                         ged,
                                         grp,
                                         0xFFFFFFFF,
                                         fdst );

        ltmpgrp = ltmpgrp->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3mesh_weightgroupWeights ( G3DEXPORTV3DATA  *ged, 
                                                     G3DWEIGHTGROUP *grp, 
                                                     uint32_t        flags, 
                                                     FILE           *fdst ) {
    G3DMESH *mes = grp->mes;
    LIST *ltmpver = mes->vertexList;
    uint32_t size = 0x00;
    uint32_t count = g3dmesh_getWeightgroupCount ( mes, grp );

    size += g3dexportv3_fwritel ( &count, fdst );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        G3DWEIGHT *wei = g3dvertex_getWeight ( ver, grp );

        if ( wei ) {
            size += g3dexportv3_fwritel ( &ver->id    , fdst );
            size += g3dexportv3_fwritef ( &wei->weight, fdst );
        }

        ltmpver = ltmpver->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3mesh_weightgroupName ( G3DEXPORTV3DATA  *ged, 
                                                  G3DWEIGHTGROUP *grp, 
                                                  uint32_t        flags, 
                                                  FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwrite ( grp->name, strlen ( grp->name ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3mesh_weightgroupEntry ( G3DEXPORTV3DATA  *ged, 
                                                 G3DWEIGHTGROUP *grp, 
                                                 uint32_t        flags, 
                                                 FILE           *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_WEIGHTGROUP_NAME,
                   EXPORTV3_CALLBACK(g3dexportv3mesh_weightgroupName),
                                     ged,
                                     grp,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_WEIGHTGROUP_WEIGHTS,
                   EXPORTV3_CALLBACK(g3dexportv3mesh_weightgroupWeights),
                                     ged,
                                     grp,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3mesh_weightgroups ( G3DEXPORTV3DATA *ged, 
                                             G3DMESH       *mes, 
                                             uint32_t       flags, 
                                             FILE          *fdst ) {
    LIST *ltmpgrp = mes->weightgroupList;
    uint32_t grpid = 0x00;
    uint32_t size = 0x00;

    while ( ltmpgrp ) {
        G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmpgrp->data;

        grp->id = grpid++; /*** for indexation by skeleton ***/

        size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_WEIGHTGROUP_ENTRY,
                       EXPORTV3_CALLBACK(g3dexportv3mesh_weightgroupEntry),
                                         ged,
                                         grp,
                                         0xFFFFFFFF,
                                         fdst );

        ltmpgrp = ltmpgrp->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3mesh_geometryPolygonsWithEdges ( G3DEXPORTV3DATA *ged, 
                                                          G3DMESH       *mes, 
                                                          uint32_t       flags, 
                                                          FILE          *fdst ) {
    LIST *ltmpfac = mes->faceList;
    uint32_t size = 0x00;
    uint32_t fid = 0x00;

    size += g3dexportv3_fwritel ( &mes->faceCount, fdst );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        /*** write twice the last vertex in case of a triangle ***/
        uint32_t idx[0x04] = { 0, 1, 2, ( fac->vertexCount == 0x03 ) ? 2 : 3 };

        fac->id = fid++;

        size += g3dexportv3_fwritel ( &fac->ver[idx[0x00]]->id, fdst );
        size += g3dexportv3_fwritel ( &fac->ver[idx[0x01]]->id, fdst );
        size += g3dexportv3_fwritel ( &fac->ver[idx[0x02]]->id, fdst );
        size += g3dexportv3_fwritel ( &fac->ver[idx[0x03]]->id, fdst );

        size += g3dexportv3_fwritel ( &fac->edg[idx[0x00]]->id, fdst );
        size += g3dexportv3_fwritel ( &fac->edg[idx[0x01]]->id, fdst );
        size += g3dexportv3_fwritel ( &fac->edg[idx[0x02]]->id, fdst );
        size += g3dexportv3_fwritel ( &fac->edg[idx[0x03]]->id, fdst );

        ltmpfac = ltmpfac->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3mesh_geometryPolygons ( G3DEXPORTV3DATA *ged, 
                                                   G3DMESH       *mes, 
                                                   uint32_t       flags, 
                                                   FILE          *fdst ) {
    LIST *ltmpfac = mes->faceList;
    uint32_t size = 0x00;
    uint32_t fid = 0x00;

    size += g3dexportv3_fwritel ( &mes->faceCount, fdst );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        /*** write twice the last vertex in case of a triangle ***/
        uint32_t idx[0x04] = { 0, 1, 2, ( fac->vertexCount == 0x03 ) ? 2 : 3 };

        fac->id = fid++;

        size += g3dexportv3_fwritel ( &fac->ver[idx[0x00]]->id, fdst );
        size += g3dexportv3_fwritel ( &fac->ver[idx[0x01]]->id, fdst );
        size += g3dexportv3_fwritel ( &fac->ver[idx[0x02]]->id, fdst );
        size += g3dexportv3_fwritel ( &fac->ver[idx[0x03]]->id, fdst );

        ltmpfac = ltmpfac->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3mesh_geometryEdges ( G3DEXPORTV3DATA *ged, 
                                              G3DMESH       *mes, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    LIST *ltmpedg = mes->edgeList;
    uint32_t size = 0x00;
    uint32_t eid = 0x00;

    size += g3dexportv3_fwritel ( &mes->edgeCount, fdst );

    while ( ltmpedg ) {
        G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;
        uint32_t flags = 0x00;

        edg->id = eid++;

        size += g3dexportv3_fwritel ( &edg->ver[0x00]->id, fdst );
        size += g3dexportv3_fwritel ( &edg->ver[0x01]->id, fdst );
        /*** Currently unused. For later use, just in case ***/
        size += g3dexportv3_fwritel ( &flags, fdst );

        ltmpedg = ltmpedg->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3mesh_geometryVertices ( G3DEXPORTV3DATA *ged, 
                                                 G3DMESH       *mes, 
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    LIST *ltmpver = mes->vertexList;
    uint32_t size = 0x00;
    uint32_t vid = 0x00;

    size += g3dexportv3_fwritel ( &mes->vertexCount, fdst );

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
        uint32_t verFlags = ver->flags & VERTEXSYMALL;

        ver->id = vid++;

        size += g3dexportv3_fwritef ( &ver->pos.x, fdst );
        size += g3dexportv3_fwritef ( &ver->pos.y, fdst );
        size += g3dexportv3_fwritef ( &ver->pos.z, fdst );
        size += g3dexportv3_fwritel ( &verFlags  , fdst );

        ltmpver = ltmpver->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3mesh_geometry ( G3DEXPORTV3DATA *ged, 
                                         G3DMESH       *mes, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;

    if ( mes->vertexList ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_GEOMETRY_VERTICES,
                       EXPORTV3_CALLBACK(g3dexportv3mesh_geometryVertices),
                                         ged,
                                         mes,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( mes->faceList ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_GEOMETRY_POLYGONS,
                       EXPORTV3_CALLBACK(g3dexportv3mesh_geometryPolygons),
                                         ged,
                                         mes,
                                         0xFFFFFFFF,
                                         fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3mesh_gouraudLimit ( G3DEXPORTV3DATA *ged, 
                                               G3DMESH         *mes, 
                                               uint32_t         flags, 
                                               FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &mes->gouraudScalarLimit, fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportv3mesh ( G3DEXPORTV3DATA *ged, 
                         G3DMESH       *mes, 
                         uint32_t       flags, 
                         FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_GOURAUDLIMIT,
                   EXPORTV3_CALLBACK(g3dexportv3mesh_gouraudLimit),
                                     ged,
                                     mes,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_GEOMETRY,
                   EXPORTV3_CALLBACK(g3dexportv3mesh_geometry),
                                     ged,
                                     mes,
                                     0xFFFFFFFF,
                                     fdst );

    if ( mes->weightgroupList ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_WEIGHTGROUPS,
                       EXPORTV3_CALLBACK(g3dexportv3mesh_weightgroups),
                                         ged,
                                         mes,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( mes->facegroupList ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_FACEGROUPS,
                       EXPORTV3_CALLBACK(g3dexportv3mesh_facegroups),
                                         ged,
                                         mes,
                                         0xFFFFFFFF,
                                         fdst );
    }

    return size;
}

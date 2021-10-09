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
    LIST *ltmpfac = grp->lfac;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &grp->nbfac, fdst );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        size += g3dexportv3_fwritel ( &fac->id, fdst );

        ltmpfac = ltmpfac->next;
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
                                   g3dexportv3mesh_facegroupName,
                                   ged,
                                   grp,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_FACEGROUP_FACES,
                                   g3dexportv3mesh_facegroupFaces,
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
    LIST *ltmpgrp = mes->lfacgrp;
    uint32_t grpid = 0x00;
    uint32_t size = 0x00;

    while ( ltmpgrp ) {
        G3DFACEGROUP *grp = ( G3DFACEGROUP * ) ltmpgrp->data;

        grp->id = grpid++; /*** for indexation by textures ***/

        size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_FACEGROUP_ENTRY,
                                       g3dexportv3mesh_facegroupEntry,
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
    LIST *ltmpwei = grp->lwei;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &grp->nbwei, fdst );

    while ( ltmpwei ) {
        G3DWEIGHT *wei = ( G3DWEIGHT * ) ltmpwei->data;

        size += g3dexportv3_fwritel ( &wei->ver->id, fdst );
        size += g3dexportv3_fwritef ( &wei->weight, fdst );

        ltmpwei = ltmpwei->next;
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
                                   g3dexportv3mesh_weightgroupName,
                                   ged,
                                   grp,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_WEIGHTGROUP_WEIGHTS,
                                   g3dexportv3mesh_weightgroupWeights,
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
    LIST *ltmpgrp = mes->lweigrp;
    uint32_t grpid = 0x00;
    uint32_t size = 0x00;

    while ( ltmpgrp ) {
        G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmpgrp->data;

        grp->id = grpid++; /*** for indexation by skeleton ***/

        size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_WEIGHTGROUP_ENTRY,
                                       g3dexportv3mesh_weightgroupEntry,
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
    LIST *ltmpfac = mes->lfac;
    uint32_t size = 0x00;
    uint32_t fid = 0x00;

    size += g3dexportv3_fwritel ( &mes->nbfac, fdst );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        /*** write twice the last vertex in case of a triangle ***/
        uint32_t idx[0x04] = { 0, 1, 2, ( fac->nbver == 0x03 ) ? 2 : 3 };

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
    LIST *ltmpfac = mes->lfac;
    uint32_t size = 0x00;
    uint32_t fid = 0x00;

    size += g3dexportv3_fwritel ( &mes->nbfac, fdst );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
        /*** write twice the last vertex in case of a triangle ***/
        uint32_t idx[0x04] = { 0, 1, 2, ( fac->nbver == 0x03 ) ? 2 : 3 };

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
    LIST *ltmpedg = mes->ledg;
    uint32_t size = 0x00;
    uint32_t eid = 0x00;

    size += g3dexportv3_fwritel ( &mes->nbedg, fdst );

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
    LIST *ltmpver = mes->lver;
    uint32_t size = 0x00;
    uint32_t vid = 0x00;

    size += g3dexportv3_fwritel ( &mes->nbver, fdst );

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

    if ( mes->lver ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_GEOMETRY_VERTICES,
                                       g3dexportv3mesh_geometryVertices,
                                       ged,
                                       mes,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mes->ledg ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_GEOMETRY_EDGES,
                                       g3dexportv3mesh_geometryEdges,
                                       ged,
                                       mes,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mes->lfac ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_GEOMETRY_POLYGONS_WITH_EDGES,
                                       g3dexportv3mesh_geometryPolygonsWithEdges,
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
                                     g3dexportv3mesh_gouraudLimit,
                                     ged,
                                     mes,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_GEOMETRY,
                                     g3dexportv3mesh_geometry,
                                     ged,
                                     mes,
                                     0xFFFFFFFF,
                                     fdst );

    if ( mes->lweigrp ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_WEIGHTGROUPS,
                                       g3dexportv3mesh_weightgroups,
                                       ged,
                                       mes,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mes->lfacgrp ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_MESH_FACEGROUPS,
                                       g3dexportv3mesh_facegroups,
                                       ged,
                                       mes,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

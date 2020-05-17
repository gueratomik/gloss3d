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
static uint32_t g3dexportmesh_facegroupFaces ( G3DEXPORTDATA *ged, 
                                               G3DFACEGROUP  *grp, 
                                               uint32_t       flags, 
                                               FILE          *fdst ) {
    LIST *ltmpfac = grp->lfac;
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &grp->nbfac, fdst );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        size += g3dexport_fwritel ( &fac->id, fdst );

        ltmpfac = ltmpfac->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_facegroupName ( G3DEXPORTDATA *ged, 
                                              G3DFACEGROUP  *grp, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( grp->name, strlen ( grp->name ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_facegroupEntry ( G3DEXPORTDATA *ged, 
                                               G3DFACEGROUP  *grp, 
                                               uint32_t       flags, 
                                               FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_MESH_FACEGROUP_NAME,
                                   g3dexportmesh_facegroupName,
                                   ged,
                                   grp,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_MESH_FACEGROUP_FACES,
                                   g3dexportmesh_facegroupFaces,
                                   ged,
                                   grp,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmesh_facegroups ( G3DEXPORTDATA *ged, 
                                           G3DMESH       *mes, 
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    LIST *ltmpgrp = mes->lfacgrp;
    uint32_t grpid = 0x00;
    uint32_t size = 0x00;

    while ( ltmpgrp ) {
        G3DFACEGROUP *grp = ( G3DFACEGROUP * ) ltmpgrp->data;

        grp->id = grpid++; /*** for indexation by textures ***/

        size += g3dexport_writeChunk ( SIG_OBJECT_MESH_FACEGROUP_ENTRY,
                                       g3dexportmesh_facegroupEntry,
                                       ged,
                                       grp,
                                       0xFFFFFFFF,
                                       fdst );

        ltmpgrp = ltmpgrp->next;
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
    LIST *ltmpgrp = mes->lweigrp;
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

    if ( mes->lweigrp ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_MESH_WEIGHTGROUPS,
                                       g3dexportmesh_weightgroups,
                                       ged,
                                       mes,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mes->lfacgrp ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_MESH_FACEGROUPS,
                                       g3dexportmesh_facegroups,
                                       ged,
                                       mes,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

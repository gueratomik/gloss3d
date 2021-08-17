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
static uint32_t g3dexportbone_rigSkinMatrix ( G3DEXPORTDATA  *ged, 
                                              G3DRIG         *rig, 
                                              uint32_t        flags, 
                                              FILE           *fdst ) {
    uint32_t size = 0x00;
    uint32_t i;

    for ( i = 0x00; i < 0x10; i++ ) {
        size += g3dexport_fwrited ( &rig->isknmatrix[i], fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportbone_rigWeightgroup ( G3DEXPORTDATA  *ged, 
                                               G3DWEIGHTGROUP *grp, 
                                               uint32_t        flags, 
                                               FILE           *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) grp->mes;
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &grp->id, fdst );
    size += g3dexport_fwritel ( &obj->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportbone_rigWeightgroups ( G3DEXPORTDATA *ged, 
                                                G3DRIG        *rig, 
                                                uint32_t       flags, 
                                                FILE          *fdst ) {
    LIST *ltmpgrp = rig->lweightgroup;
    uint32_t size = 0x00;

    while ( ltmpgrp ) {
        G3DWEIGHTGROUP *grp = ( G3DRIG * ) ltmpgrp->data;

        size += g3dexport_writeChunk ( SIG_OBJECT_BONE_RIG_WEIGHTGROUP_ENTRY,
                                       g3dexportbone_rigWeightgroup,
                                       ged,
                                       grp,
                                       0xFFFFFFFF,
                                       fdst );

        ltmpgrp = ltmpgrp->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportbone_rig ( G3DEXPORTDATA *ged, 
                                    G3DRIG        *rig, 
                                    uint32_t       flags, 
                                    FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_BONE_RIG_WEIGHTGROUPS,
                                   g3dexportbone_rigWeightgroups,
                                   ged,
                                   rig,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_BONE_RIG_SKINMATRIX,
                                   g3dexportbone_rigSkinMatrix,
                                   ged,
                                   rig,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportbone_rigs ( G3DEXPORTDATA *ged, 
                                     G3DBONE       *bon, 
                                     uint32_t       flags, 
                                     FILE          *fdst ) {
    uint32_t size = 0x00;
    LIST *ltmprig = bon->lrig;

    while ( ltmprig ) {
        G3DRIG *rig = ( G3DRIG * ) ltmprig->data;

        size += g3dexport_writeChunk ( SIG_OBJECT_BONE_RIG_ENTRY,
                                       g3dexportbone_rig,
                                       ged,
                                       rig,
                                       0xFFFFFFFF,
                                       fdst );

        ltmprig = ltmprig->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportbone_fixingTransformation ( G3DEXPORTDATA *ged, 
                                                     G3DBONE       *bon, 
                                                     uint32_t       flags, 
                                                     FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &bon->fixpos.x, fdst );
    size += g3dexport_fwritef ( &bon->fixpos.y, fdst );
    size += g3dexport_fwritef ( &bon->fixpos.z, fdst );
    size += g3dexport_fwritef ( &bon->fixpos.w, fdst );

    size += g3dexport_fwritef ( &bon->fixrot.x, fdst );
    size += g3dexport_fwritef ( &bon->fixrot.y, fdst );
    size += g3dexport_fwritef ( &bon->fixrot.z, fdst );
    size += g3dexport_fwritef ( &bon->fixrot.w, fdst );

    size += g3dexport_fwritef ( &bon->fixsca.x, fdst );
    size += g3dexport_fwritef ( &bon->fixsca.y, fdst );
    size += g3dexport_fwritef ( &bon->fixsca.z, fdst );
    size += g3dexport_fwritef ( &bon->fixsca.w, fdst );

    return size;
}


/******************************************************************************/
static uint32_t g3dexportbone_fixing ( G3DEXPORTDATA *ged, 
                                       G3DBONE       *bon, 
                                       uint32_t       flags, 
                                       FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_BONE_FIXING_TRANSFORMATION,
                                   g3dexportbone_fixingTransformation,
                                   ged,
                                   bon,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportbone_length ( G3DEXPORTDATA *ged, 
                                       G3DBONE       *bon, 
                                       uint32_t       flags, 
                                       FILE          *fdst ) {
    return g3dexport_fwritef ( &bon->len, fdst );
}

/******************************************************************************/
uint32_t g3dexportbone ( G3DEXPORTDATA *ged, 
                         G3DBONE       *bon, 
                         uint32_t       flags, 
                         FILE          *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) bon;
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_BONE_LENGTH,
                                   g3dexportbone_length,
                                   ged,
                                   bon,
                                   0xFFFFFFFF,
                                   fdst );

    if ( obj->flags & BONEFIXED ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_BONE_FIXING,
                                       g3dexportbone_fixing,
                                       ged,
                                       bon,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( bon->lrig ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_BONE_RIGS,
                                       g3dexportbone_rigs,
                                       ged,
                                       bon,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

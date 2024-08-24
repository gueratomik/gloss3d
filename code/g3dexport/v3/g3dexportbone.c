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
static uint32_t g3dexportv3bone_rigSkin ( G3DEXPORTV3DATA  *ged, 
                                        G3DRIG         *rig, 
                                        uint32_t        flags, 
                                        FILE           *fdst ) {
    uint32_t size = 0x00;
    uint32_t i;

    size += g3dexportv3_fwritel ( &((G3DOBJECT*)rig->skn)->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3bone_rigSkinMatrix ( G3DEXPORTV3DATA  *ged, 
                                                G3DRIG           *rig, 
                                                uint32_t          flags, 
                                                FILE             *fdst ) {
    uint32_t size = 0x00;
    uint32_t i;

    for ( i = 0x00; i < 0x10; i++ ) {
        double cell = rig->isknmatrix[i];

        size += g3dexportv3_fwrited ( &cell, fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3bone_rigWeightgroup ( G3DEXPORTV3DATA *ged, 
                                                 G3DWEIGHTGROUP  *grp, 
                                                 uint32_t         flags, 
                                                 FILE            *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) grp->mes;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &grp->id, fdst );
    size += g3dexportv3_fwritel ( &obj->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3bone_rigWeightgroups ( G3DEXPORTV3DATA *ged, 
                                                G3DRIG        *rig, 
                                                uint32_t       flags, 
                                                FILE          *fdst ) {
    LIST *ltmpgrp = rig->lweightgroup;
    uint32_t size = 0x00;

    while ( ltmpgrp ) {
        G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmpgrp->data;

        size += g3dexportv3_writeChunk ( SIG_OBJECT_BONE_RIG_WEIGHTGROUP_ENTRY,
                       EXPORTV3_CALLBACK(g3dexportv3bone_rigWeightgroup),
                                         ged,
                                         grp,
                                         0xFFFFFFFF,
                                         fdst );

        ltmpgrp = ltmpgrp->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3bone_rig ( G3DEXPORTV3DATA *ged, 
                                    G3DRIG        *rig, 
                                    uint32_t       flags, 
                                    FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_BONE_RIG_SKIN,
                   EXPORTV3_CALLBACK(g3dexportv3bone_rigSkin),
                                     ged,
                                     rig,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_BONE_RIG_WEIGHTGROUPS,
                   EXPORTV3_CALLBACK(g3dexportv3bone_rigWeightgroups),
                                     ged,
                                     rig,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_BONE_RIG_SKINMATRIX,
                   EXPORTV3_CALLBACK(g3dexportv3bone_rigSkinMatrix),
                                     ged,
                                     rig,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3bone_rigs ( G3DEXPORTV3DATA *ged, 
                                     G3DBONE       *bon, 
                                     uint32_t       flags, 
                                     FILE          *fdst ) {
    uint32_t size = 0x00;
    LIST *ltmprig = bon->lrig;

    while ( ltmprig ) {
        G3DRIG *rig = ( G3DRIG * ) ltmprig->data;

        size += g3dexportv3_writeChunk ( SIG_OBJECT_BONE_RIG_ENTRY,
                       EXPORTV3_CALLBACK(g3dexportv3bone_rig),
                                         ged,
                                         rig,
                                         0xFFFFFFFF,
                                         fdst );

        ltmprig = ltmprig->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3bone_fixingTransformation ( G3DEXPORTV3DATA *ged, 
                                                     G3DBONE       *bon, 
                                                     uint32_t       flags, 
                                                     FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &bon->fixpos.x, fdst );
    size += g3dexportv3_fwritef ( &bon->fixpos.y, fdst );
    size += g3dexportv3_fwritef ( &bon->fixpos.z, fdst );
    size += g3dexportv3_fwritef ( &bon->fixpos.w, fdst );

    size += g3dexportv3_fwritef ( &bon->fixrot.x, fdst );
    size += g3dexportv3_fwritef ( &bon->fixrot.y, fdst );
    size += g3dexportv3_fwritef ( &bon->fixrot.z, fdst );
    size += g3dexportv3_fwritef ( &bon->fixrot.w, fdst );

    size += g3dexportv3_fwritef ( &bon->fixsca.x, fdst );
    size += g3dexportv3_fwritef ( &bon->fixsca.y, fdst );
    size += g3dexportv3_fwritef ( &bon->fixsca.z, fdst );
    size += g3dexportv3_fwritef ( &bon->fixsca.w, fdst );

    return size;
}


/******************************************************************************/
static uint32_t g3dexportv3bone_fixing ( G3DEXPORTV3DATA *ged, 
                                       G3DBONE       *bon, 
                                       uint32_t       flags, 
                                       FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_BONE_FIXING_TRANSFORMATION,
                   EXPORTV3_CALLBACK(g3dexportv3bone_fixingTransformation),
                                     ged,
                                     bon,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3bone_length ( G3DEXPORTV3DATA *ged, 
                                       G3DBONE       *bon, 
                                       uint32_t       flags, 
                                       FILE          *fdst ) {
    return g3dexportv3_fwritef ( &bon->len, fdst );
}

/******************************************************************************/
uint32_t g3dexportv3bone ( G3DEXPORTV3DATA *ged, 
                         G3DBONE       *bon, 
                         uint32_t       flags, 
                         FILE          *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) bon;
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_BONE_LENGTH,
                   EXPORTV3_CALLBACK(g3dexportv3bone_length),
                                     ged,
                                     bon,
                                     0xFFFFFFFF,
                                     fdst );

    if ( obj->flags & BONEFIXED ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_BONE_FIXING,
                       EXPORTV3_CALLBACK(g3dexportv3bone_fixing),
                                         ged,
                                         bon,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( bon->lrig ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_BONE_RIGS,
                       EXPORTV3_CALLBACK(g3dexportv3bone_rigs),
                                         ged,
                                         bon,
                                         0xFFFFFFFF,
                                         fdst );
    }

    return size;
}

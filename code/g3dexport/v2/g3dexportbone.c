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
static uint32_t g3dexportv2bone_rigSkin ( G3DEXPORTV2DATA  *ged, 
                                        G3DRIG         *rig, 
                                        uint32_t        flags, 
                                        FILE           *fdst ) {
    uint32_t size = 0x00;
    uint32_t i;

    size += g3dexportv2_fwritel ( &((G3DOBJECT*)rig->skn)->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2bone_rigSkinMatrix ( G3DEXPORTV2DATA  *ged, 
                                              G3DRIG         *rig, 
                                              uint32_t        flags, 
                                              FILE           *fdst ) {
    uint32_t size = 0x00;
    uint32_t i;

    for ( i = 0x00; i < 0x10; i++ ) {
        size += g3dexportv2_fwrited ( &rig->isknmatrix[i], fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2bone_rigWeightgroup ( G3DEXPORTV2DATA  *ged, 
                                               G3DWEIGHTGROUP *grp, 
                                               uint32_t        flags, 
                                               FILE           *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) grp->mes;
    uint32_t size = 0x00;

    size += g3dexportv2_fwritel ( &grp->id, fdst );
    size += g3dexportv2_fwritel ( &obj->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2bone_rigWeightgroups ( G3DEXPORTV2DATA *ged, 
                                                G3DRIG        *rig, 
                                                uint32_t       flags, 
                                                FILE          *fdst ) {
    LIST *ltmpgrp = rig->lweightgroup;
    uint32_t size = 0x00;

    while ( ltmpgrp ) {
        G3DWEIGHTGROUP *grp = ( G3DRIG * ) ltmpgrp->data;

        size += g3dexportv2_writeChunk ( SIG_OBJECT_BONE_RIG_WEIGHTGROUP_ENTRY,
                                       g3dexportv2bone_rigWeightgroup,
                                       ged,
                                       grp,
                                       0xFFFFFFFF,
                                       fdst );

        ltmpgrp = ltmpgrp->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2bone_rig ( G3DEXPORTV2DATA *ged, 
                                    G3DRIG        *rig, 
                                    uint32_t       flags, 
                                    FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_OBJECT_BONE_RIG_SKIN,
                                   g3dexportv2bone_rigSkin,
                                   ged,
                                   rig,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv2_writeChunk ( SIG_OBJECT_BONE_RIG_WEIGHTGROUPS,
                                   g3dexportv2bone_rigWeightgroups,
                                   ged,
                                   rig,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv2_writeChunk ( SIG_OBJECT_BONE_RIG_SKINMATRIX,
                                   g3dexportv2bone_rigSkinMatrix,
                                   ged,
                                   rig,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2bone_rigs ( G3DEXPORTV2DATA *ged, 
                                     G3DBONE       *bon, 
                                     uint32_t       flags, 
                                     FILE          *fdst ) {
    uint32_t size = 0x00;
    LIST *ltmprig = bon->lrig;

    while ( ltmprig ) {
        G3DRIG *rig = ( G3DRIG * ) ltmprig->data;

        size += g3dexportv2_writeChunk ( SIG_OBJECT_BONE_RIG_ENTRY,
                                       g3dexportv2bone_rig,
                                       ged,
                                       rig,
                                       0xFFFFFFFF,
                                       fdst );

        ltmprig = ltmprig->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2bone_fixingTransformation ( G3DEXPORTV2DATA *ged, 
                                                     G3DBONE       *bon, 
                                                     uint32_t       flags, 
                                                     FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_fwritef ( &bon->fixpos.x, fdst );
    size += g3dexportv2_fwritef ( &bon->fixpos.y, fdst );
    size += g3dexportv2_fwritef ( &bon->fixpos.z, fdst );
    size += g3dexportv2_fwritef ( &bon->fixpos.w, fdst );

    size += g3dexportv2_fwritef ( &bon->fixrot.x, fdst );
    size += g3dexportv2_fwritef ( &bon->fixrot.y, fdst );
    size += g3dexportv2_fwritef ( &bon->fixrot.z, fdst );
    size += g3dexportv2_fwritef ( &bon->fixrot.w, fdst );

    size += g3dexportv2_fwritef ( &bon->fixsca.x, fdst );
    size += g3dexportv2_fwritef ( &bon->fixsca.y, fdst );
    size += g3dexportv2_fwritef ( &bon->fixsca.z, fdst );
    size += g3dexportv2_fwritef ( &bon->fixsca.w, fdst );

    return size;
}


/******************************************************************************/
static uint32_t g3dexportv2bone_fixing ( G3DEXPORTV2DATA *ged, 
                                       G3DBONE       *bon, 
                                       uint32_t       flags, 
                                       FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_OBJECT_BONE_FIXING_TRANSFORMATION,
                                   g3dexportv2bone_fixingTransformation,
                                   ged,
                                   bon,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2bone_length ( G3DEXPORTV2DATA *ged, 
                                       G3DBONE       *bon, 
                                       uint32_t       flags, 
                                       FILE          *fdst ) {
    return g3dexportv2_fwritef ( &bon->len, fdst );
}

/******************************************************************************/
uint32_t g3dexportv2bone ( G3DEXPORTV2DATA *ged, 
                         G3DBONE       *bon, 
                         uint32_t       flags, 
                         FILE          *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) bon;
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_OBJECT_BONE_LENGTH,
                                   g3dexportv2bone_length,
                                   ged,
                                   bon,
                                   0xFFFFFFFF,
                                   fdst );

    if ( obj->flags & BONEFIXED ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_BONE_FIXING,
                                       g3dexportv2bone_fixing,
                                       ged,
                                       bon,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( bon->lrig ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_BONE_RIGS,
                                       g3dexportv2bone_rigs,
                                       ged,
                                       bon,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}
